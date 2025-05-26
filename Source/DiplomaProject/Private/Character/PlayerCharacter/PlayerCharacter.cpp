// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter/PlayerCharacter.h"

// UE5 class
#include "Kismet/KismetSystemLibrary.h"

// UE5 Component class
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PoseSearch/PoseSearchTrajectoryTypes.h"

// Custom Component class
#include "Component/MovementComponent/Player_CMC.h"
//#include "Component/TrajectoryComponent/Player_CTC.h"

// Custom Debug.h
#include "../../../DebugHelper.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayer_CMC>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Original MovementComponent Converted to Custom MovementComponent
	MovementComponent = Cast<UPlayer_CMC>(GetMovementComponent());
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->bUseControllerDesiredRotation = true;
	MovementComponent->MaxWalkSpeed = 300.f;	

	//// Create Trajectory Component
	//TrajectoryComponent = CreateDefaultSubobject<UPlayer_CTC>(TEXT("Player CTC"));

	// Create SpringArm and Camera Components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;


	OnCharacterMovementUpdated.AddDynamic(this, &APlayerCharacter::CharacterMovementUpdatedEvent);
}


// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CustomOnLandedEvent_LatentInfo.Linkage = 0;
	CustomOnLandedEvent_LatentInfo.UUID = GetUniqueID();
	CustomOnLandedEvent_LatentInfo.ExecutionFunction = TEXT("CustomOnLandedEvent_OnDelayFinished");
	CustomOnLandedEvent_LatentInfo.CallbackTarget = this;

}

// Delay struct parameter & Bind Function
/*-------------------------------CustomOnLandedEvent Delay-------------------------------*/
void APlayerCharacter::CustomOnLandedEvent_OnDelayFinished() { bJustedLanded = false; }



// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Draw character vector direction and Camera vector direction in Tick
	DrawCharacterVectorDirection();
	DrawCameraVectorDirection();
}


void APlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainMappingContext, 0);
		}
	}
}


// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set Action Bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		
		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	
		// Switch Character Movement Mode
		EnhancedInputComponent->BindAction(SwitchMovementModeAction, ETriggerEvent::Completed, this, &APlayerCharacter::SwitchMovementMode);

		// Switch Character Walk or Run
		EnhancedInputComponent->BindAction(SwitchWalkorRunAction, ETriggerEvent::Completed, this, &APlayerCharacter::SwitchWalkorRun);
	}
}


#pragma region Input Variable & Function
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// Input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward Vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// Get Right Vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add Movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


/*
Switch Character Movement Mode :
1. Movement based on Camera orientation
2. Movement based on character orientation
*/ 
void APlayerCharacter::SwitchMovementMode()
{
	SwitchtoCharacterOrientation = !SwitchtoCharacterOrientation;
	MovementComponent->bOrientRotationToMovement = SwitchtoCharacterOrientation ? true : false;
	MovementComponent->bUseControllerDesiredRotation = SwitchtoCharacterOrientation ? false : true;
}

void APlayerCharacter::SwitchWalkorRun()
{
	Gait = (Gait == EGait::Walk) ? EGait::Run : EGait::Walk;
	SetGait(Gait);
}

#pragma endregion


#pragma region Custom Enum & Function
// MovementStates, Gait, MovementPosition Get Function
EMovementStates APlayerCharacter::GetMovementStates() { return MovementStates; }

EGait APlayerCharacter::GetGait() { return Gait; }

EMovementPosition APlayerCharacter::GetMovementPosition() { return MovementMode; }

ERotationMode APlayerCharacter::GetRotationMode() { return RotationMode; }



// MovementStates, Gait, MovementPosition Set Function
void APlayerCharacter::SetMovementStates(EMovementStates NewMovementStates)
{
	MovementStates = NewMovementStates;
	OnMovementStateChanged.Broadcast(NewMovementStates);
}

void APlayerCharacter::SetGait(EGait NewGait)
{
	Gait = NewGait;
	MovementComponent->MaxWalkSpeed = (Gait == EGait::Walk) ? 300.f : 500.f;
	OnGaitChanged.Broadcast(NewGait);
}

void APlayerCharacter::SetMovementPosition(EMovementPosition NewMovementMode)
{
	MovementMode = NewMovementMode;
	OnMovementPositionChanged.Broadcast(NewMovementMode);
}

void APlayerCharacter::SetRotationMode(ERotationMode NewRotationMode)
{
	RotationMode = NewRotationMode;
	OnRotationModeChanged.Broadcast(NewRotationMode);
}


void APlayerCharacter::TraceToGround()
{
	FHitResult LineTraceHitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + FVector(0, 0, -2000.f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel
	(
		LineTraceHitResult,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);

	if (bHit)
	{
		CalculateTimeToLand(StartLocation, LineTraceHitResult);
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.f, 0, 1.f);
	}

}

float APlayerCharacter::CalculateTimeToLand(FVector StartLocation, FHitResult LineTraceHitResult)
{
	FVector LandingLocation = LineTraceHitResult.Location;
	float RemainingHeight = StartLocation.Z - LandingLocation.Z;
	TimeToLand = (2 * RemainingHeight) / FMath::Abs(MovementComponent->GetGravityZ());

	return TimeToLand;
}
#pragma endregion


#pragma region About Jump & Land
void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	CustomOnLandedEvent(MovementComponent->Velocity);
}


void APlayerCharacter::CharacterMovementUpdatedEvent(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	UpdatedMovementSimulated(OldVelocity);
}


void APlayerCharacter::UpdatedMovementSimulated(FVector OldVelocity)
{
	bool bIsMovingOnGround = MovementComponent->IsMovingOnGround();

	bool bWasMovingOnGroundLastFarmeSimulated = bIsMovingOnGround;

	if (bIsMovingOnGround != bWasMovingOnGroundLastFarmeSimulated)
	{
		if (bIsMovingOnGround) { CustomOnLandedEvent(OldVelocity); }
	}
}


void APlayerCharacter::CustomOnLandedEvent(FVector OldVelocity)
{
	LandVelocity = OldVelocity;
	bJustedLanded = true;
	UKismetSystemLibrary::Delay(GetWorld(), 0.1f, CustomOnLandedEvent_LatentInfo);
}
#pragma endregion



#pragma region Debug
void APlayerCharacter::DrawCharacterVectorDirection()
{
	// Get Vector for Character Move Direction
	FVector Velocity = GetVelocity();
	Debug::PrintVector("Character Velocity: ", Velocity, 0.f, false, FColor::Green);

	// Get Character Speed
	float Speed = Velocity.Size();
	// Use Normalizer for Calculate Character Move Direction
	FVector Direction = Velocity.GetSafeNormal();

	if (Speed > 0.f)
	{
		// Arrow will be draw from the bottom of the player
		FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 85.f);
		// Arrow will point in the direction of the player movement
		FVector End = Start + Direction * (Speed * 0.1f);

		// Draw Arrow for Character Move Direction Vector
		UKismetSystemLibrary::DrawDebugArrow(this, Start, End, 40.f, FLinearColor::Green, 0.f, 3.f);
	}
	
}

void APlayerCharacter::DrawCameraVectorDirection()
{
	// Get Camera Rotation
	FRotator CameraRotation = PlayerController->GetControlRotation();
	// Get Camera Forward Vector
	FVector CameraForwardVector = CameraRotation.Vector();

	// Arrow will be draw from the bottom of the player
	FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 85.f);
	// Arrow will point in the direction of the player movement
	FVector End = Start + CameraForwardVector * 100.f;

	// Draw Arrow for Camera Forward Vector
	UKismetSystemLibrary::DrawDebugArrow(this, Start, End, 40.f, FLinearColor::Red, 0.f, 3.f);
}
#pragma endregion