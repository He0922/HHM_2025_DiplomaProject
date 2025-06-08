// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/AnimInstance/MyAnimInstance.h"
#include "Character/PlayerCharacter/MyCharacter.h"

// My Component
#include "Component/MovementComponent/Player_CMC.h"

#include "PoseSearch/PoseSearchTrajectoryLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

// My Misc
#include "../../../DebugHelper.h"

// My Enum
#include "Enum/GaitEnum.h"
#include "Enum/MovementStatesEnum.h"
#include "Enum/MovementPositionEnum.h"
#include "Enum/RotationModeEnum.h"


void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<AMyCharacter>(TryGetPawnOwner());

	if (PlayerCharacter)
	{	
		// Subscribe to Listen Events
		PlayerCharacter->MovementStateChanged.AddDynamic(this, &UMyAnimInstance::MovementStateChanged);
		PlayerCharacter->GaitChanged.AddDynamic(this, &UMyAnimInstance::GaitChanged);
		PlayerCharacter->MovementPositionChanged.AddDynamic(this, &UMyAnimInstance::MovementPositionChanged);
		PlayerCharacter->RotationModeChanged.AddDynamic(this, &UMyAnimInstance::RotationModeChanged);

		PlayerCharacter->IsLandedChanged.AddDynamic(this, &UMyAnimInstance::IsJustLandedChanged);
		PlayerCharacter->LandVelocityChanged.AddDynamic(this, &UMyAnimInstance::LandVelocityChanged);
	
		MovementComponent = PlayerCharacter->MovementComponent;
	}


	if (!MovementComponent)
	{
		Debug::Print("Movement Component Is NULL", 5.f, false, FColor::Red);
	}
}


void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!PlayerCharacter || !MovementComponent) return;

	GenerateTrajectory();
	UpdateNecessaryVariable();
	UpdateState();

	EnumDebug();
	VariablesDebug();
}


/*===================================== Listener Event =====================================*/
void UMyAnimInstance::MovementStateChanged(EMovementStates NewMovementState) { MovementState = NewMovementState; }

void UMyAnimInstance::GaitChanged(EGait NewGait) { Gait = NewGait; }

void UMyAnimInstance::MovementPositionChanged(EMovementPosition NewMovementPosition) { MovementPosition = NewMovementPosition; }

void UMyAnimInstance::RotationModeChanged(ERotationMode NewRotationMode) { RotationMode = NewRotationMode; }



/*================================== Generate Trajectory ===================================*/
void UMyAnimInstance::GenerateTrajectory(float HistorySamplingInterval, int HistorySamplingCount,
										 float PredictionSamplingInterval, int PredictionSamplingCount,
										 float DebugThickness, float HeightOffset)
{
	AMyCharacter* OwningCharacter = Cast<AMyCharacter>(GetOwningActor());
	if (!OwningCharacter) return;

	DeltaTime = GetWorld()->GetDeltaSeconds();

	InOutDesiredControllerYawLastUpdate = OwningCharacter->GetControlRotation().Yaw;

	// Call PoseSearchLibraray Generate Trajectory
	UPoseSearchTrajectoryLibrary::PoseSearchGenerateTrajectory
	(
		this,
		InTrajectoryData,
		DeltaTime,
		InOutTrajectory,
		InOutDesiredControllerYawLastUpdate,
		Trajectory,
		HistorySamplingInterval,
		HistorySamplingCount,
		PredictionSamplingInterval,
		PredictionSamplingCount
	);
	

	//Trajectory.DebugDrawTrajectory(GetWorld(), DebugThickness, HeightOffset);
}


/*================================== Necessary Variable ===================================*/
void UMyAnimInstance::UpdateNecessaryVariable()
{
	vVelocity = MovementComponent->Velocity;
	fSpeed2D = vVelocity.Size2D();
	GetFutureVelocity();
	IsMoving();
	IsStarting();
	IsPivoting();
	UpdateLandingType();
}


FVector UMyAnimInstance::GetFutureVelocity(float FutureTime)
{
	UPoseSearchTrajectoryLibrary::GetTrajectoryVelocity(Trajectory, 0.f, FutureTime, vFutureVelocity);

	return vFutureVelocity;
}


void UMyAnimInstance::IsStarting(float StartThreshold)
{
	bIsMoving = IsMoving();
	if (!bIsMoving)
	{
		bIsStarting = false;
		return;
	}

	float fVelocityDiff = FMath::Abs((vFutureVelocity - vVelocity).Size2D());
	bool bVelocityChangeExceeded = fVelocityDiff > StartThreshold;

	bIsStarting = bVelocityChangeExceeded && bIsMoving;
}


bool UMyAnimInstance::IsMoving()
{
	bool bFutureVelocity = !vFutureVelocity.Equals(FVector(0, 0, 0), 0.1f);
	bool bVelocity = !vVelocity.Equals(FVector(0, 0, 0), 0.1f);

	bIsMoving = bFutureVelocity && bVelocity;
	return bIsMoving;
}


void UMyAnimInstance::IsPivoting(float RotationThreshold)
{
	FRotator rFurtureRotation = UKismetMathLibrary::MakeRotFromX(vFutureVelocity);
	FRotator rVeclocity = UKismetMathLibrary::MakeRotFromX(vVelocity);
	FRotator rRotator = UKismetMathLibrary::NormalizedDeltaRotator(rFurtureRotation, rVeclocity);

	float fRotatorYaw = UKismetMathLibrary::Abs(rRotator.Yaw);
	

	if (fRotatorYaw >= RotationThreshold && bIsStarting && !bIsPivoting)
	{
		bIsPivoting = true;
		PivotTimer = 0.f;
	}

	if (bIsPivoting)
	{
		PivotTimer += 1.f;
		if (!bIsStarting || PivotTimer > 120.f)
		{
			bIsPivoting = false;
			PivotTimer = 0.f;
		}
	}
}


// Land
void UMyAnimInstance::UpdateLandingType(float LightThreshold)
{
	if (!bIsJustLanded)
	{
		LandedType = ELandedType::None;
		return;
	}
	float LandingSpeed = FMath::Abs(vLandVelocity.Z);

	Debug::PrintFloat("LandingSpeed: ", LandingSpeed, 0.f, false, FColor::Red);

	if (LandingSpeed > LightThreshold)
	{
		LandedType = ELandedType::Heavy;
	}
	else
	{
		LandedType = ELandedType::Light;
	}
}


void UMyAnimInstance::IsJustLandedChanged(bool NewIsJustLanded)
{ 
	bIsJustLanded = NewIsJustLanded; 
	
}

void UMyAnimInstance::LandVelocityChanged(FVector NewLandVelocity)
{
	vLandVelocity = NewLandVelocity;
}


/*==================================== Update State =====================================*/
void UMyAnimInstance::UpdateState()
{
	UpdateMovementState();
	UpdateMovementPosition();
}


void UMyAnimInstance::UpdateMovementState()
{
	EMovementStates NewMovementState = IsMoving() ? EMovementStates::Moving : EMovementStates::Idle;
	PlayerCharacter->SetMovementState(NewMovementState);
}

// Character Controls the State of Gait
void UMyAnimInstance::UpdateGait()
{

}


void UMyAnimInstance::UpdateMovementPosition()
{
	switch (MovementComponent->MovementMode)
	{
	case MOVE_Walking:
		PlayerCharacter->SetMovementPosition(EMovementPosition::OnGround);
		break;
	case MOVE_Falling:
		PlayerCharacter->SetMovementPosition(EMovementPosition::InAir);
		break;
	}
}

// Character Controls the State of RotationMode
void UMyAnimInstance::UpdateRotationMode()
{

}



void UMyAnimInstance::EnumDebug()
{
	Debug::Print("   ", 0.f, true);
	Debug::Print("   ", 0.f, true);
	Debug::Print("   ", 0.f, true);
	Debug::Print("   ", 0.f, true);
	Debug::Print("   ", 0.f, true);
	Debug::Print("   ", 0.f, true);

	Debug::PrintEnum("Movement State: ", MovementState, 0.f, true, FColor::Green);
	Debug::PrintEnum("Gait: ", Gait, 0.f, true, FColor::Cyan);
	Debug::PrintEnum("Movvement Position: ", MovementPosition, 0.f, true, FColor::Green);
	Debug::PrintEnum("Rotation Mode: ", RotationMode, 0.f, true, FColor::Green);
	Debug::PrintEnum("Landed Type: ", LandedType, 0.f, true, FColor::Green);
}


void UMyAnimInstance::VariablesDebug()
{

	//Debug::Print("   ", 0.f, false);
	//Debug::Print("   ", 0.f, false);
	//Debug::Print("   ", 0.f, false);
	//Debug::PrintEnum("Landed Type: ", LandedType, 0.f, false, FColor::Cyan);
	//Debug::PrintBool("Just Landed: ", bIsJustLanded, 0.f, false, FColor::Red);
	//Debug::PrintFloat("fSpeed 2D: ", fSpeed2D, 0.f, false, FColor::Cyan);
}

