// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/AnimInstance/Player_AnimInstance.h"

// UE5 Libaray
#include "Math/UnrealMathUtility.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h" // use Pose Search Generate Trajectroy(forCharacter)
#include "DrawDebugHelpers.h"

// Custom Class include
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "Component/TrajectoryComponent/Player_CTC.h"
#include "Component/MovementComponent/Player_CMC.h"



void UPlayer_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Player = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (Player)
	{
		// Listener event binding
		Player->OnMovementStateChanged.AddDynamic(this, &UPlayer_AnimInstance::OnMovementStateChanged);
		Player->OnGaitChanged.AddDynamic(this, &UPlayer_AnimInstance::OnGaitChanged);
		Player->OnMovementPositionChanged.AddDynamic(this, &UPlayer_AnimInstance::OnMovementPositionChanged);
		Player->OnRotationModeChanged.AddDynamic(this, &UPlayer_AnimInstance::OnRotationModeChanged);
	}
}

void UPlayer_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Player)
	{
		// Trajectory = Player->TrajectoryComponent;
		MovementComponent = Player->MovementComponent;
	}
}



void UPlayer_AnimInstance::OnMovementStateChanged(EMovementStates NewMovementState) { MovementState = NewMovementState; }

void UPlayer_AnimInstance::OnGaitChanged(EGait NewGait) { Gait = NewGait; }

void UPlayer_AnimInstance::OnMovementPositionChanged(EMovementPosition NewMovementMode) { MovementMode = NewMovementMode; }

void UPlayer_AnimInstance::OnRotationModeChanged(ERotationMode NewRotationMode) { RotationMode = NewRotationMode; }


#pragma region GenerateTrajectory Necessity Variable
/*-------------------------------------Generate Trajectory -------------------------------------------*/
// ���ɹ켣���ݣ�������δ���ٶ�
void UPlayer_AnimInstance::GenerateTrajectory(float HistorySamplingInterval, int HistorySamplingCount,
												float PredictionSamplingInterval, int PredictionSamplingCount,
												float DebugThickness, float HeightOffset)
{
	APlayerCharacter* OwningCharacter = Cast<APlayerCharacter>(GetOwningActor());
	if (!OwningCharacter) return;

	DeltaTime = GetWorld()->GetDeltaSeconds();

	InOutDesiredControllerYawLastUpdate = OwningCharacter->GetControlRotation().Yaw;

	// ʹ��PoseSearchGenerateTrajectory�������ɹ켣����
	UPoseSearchTrajectoryLibrary::PoseSearchGenerateTrajectory
	(
		this,
		TrajectoryGenerateDataMoving,
		DeltaTime,
		InOutTrajectory,
		InOutDesiredControllerYawLastUpdate,
		Trajectory,
		HistorySamplingInterval,
		HistorySamplingCount,
		PredictionSamplingInterval,
		PredictionSamplingCount
	);

	

	// Trajectory��δ����0.5s��0.4sλ�ü����δ���ٶ�(���ڹ켣���λ�Ʊ仯)
	/*
	Ϊʲô��ѡ������ʱ����: 
	1. ��Ӧ�ٶȱ���: ���ʱ����������FutureVelocity�����޷���ʱ��ӳ��ɫ�ļ��ٶȻ����״̬
	2. �������ڸ�Ƶ����ƥ��: �����Ϸ�Ķ�������Ƶ�ʽϸ�, ��ʱ�������ܵ��¶����л��ͺ�
	*/
	vFutureVelocity = (Trajectory.GetSampleAtTime(0.5f).Position - Trajectory.GetSampleAtTime(0.4f).Position) * 10;
	
	Trajectory.DebugDrawTrajectory(GetWorld(), DebugThickness, HeightOffset);
}


/*-------------------------------------Update Necessray Variable -------------------------------------------*/
void UPlayer_AnimInstance::UpdateNecessaryVariables()
{
	if (!MovementComponent) return;
	vVelocity = MovementComponent->Velocity;
	fSpeed2D = vVelocity.Size2D();
	bIsFalling = MovementComponent->IsFalling();
	MovementMode = bIsFalling ? EMovementPosition::InAir : EMovementPosition::OnGround;
	JustLanded_Light();
	JustLanded_Heavy();
}


bool UPlayer_AnimInstance::IsMoving()
{
	bool bVelocity = (!vVelocity.Equals(FVector(0, 0, 0), 0.1f)) ? true : false;
	bool bFutureVelocity = (!vFutureVelocity.Equals(FVector(0, 0, 0), 0.1f)) ? true : false;
	
	bIsMoving = bVelocity && bFutureVelocity;

	return bIsMoving;
}


bool UPlayer_AnimInstance::IsStarting()
{
	// ��δ���ٶȴ��ڵ�ǰ�ٶ�����100��˵��Character���ڼ��٣����δ���ٶ�С��(��ǰ�ٶ�+100)��˵��Character�Ѿ������������˶�
	// FutureVelocity��������²������MaxWalkSpeed����ΪFutureVelocity��PoseSearchGenerateTrajectory���ɵģ�
	// ��ͨ������ѭCharacterMovementComponent���ٶ�����
	bool Judgment1 = vFutureVelocity.Size2D() >= (vVelocity.Size2D() + 100.f);
	bool Judgment2 = IsMoving();

	bIsStarting = Judgment1 && Judgment2;

	return bIsStarting;
}


/*-------------------------------------Update State-------------------------------------------*/
void UPlayer_AnimInstance::UpdateState()
{
	// ͨ��MovementComponent�е�MovementMode�����Զ����MovementPositionö�ٱ����ı䣬
	// ������Character���а󶨿ո񰴼��Ըı�ö�ٱ���
	// Update MovementPosition
	switch (MovementComponent->MovementMode)
	{
	case MOVE_Walking:
		Player->SetMovementPosition(EMovementPosition::OnGround);
		break;
	case MOVE_Falling:
		Player->SetMovementPosition(EMovementPosition::InAir);
		break;
	default:
		Player->SetMovementPosition(EMovementPosition::OnGround);
		break;
	}

	// Update RootationMode
	if (MovementComponent->bOrientRotationToMovement) { Player->SetRotationMode(ERotationMode::OrientToMovement); }
	else { Player->SetRotationMode(ERotationMode::Strafe); }

	// Update MovementStates
	bool IsMovingJudgment = IsMoving();
	bool IsStartingJudgment = IsStarting();
	EMovementStates NewMovementState = IsMovingJudgment ? EMovementStates::Moving : EMovementStates::Idle;
	Player->SetMovementStates(NewMovementState);

	// Update Gait
	switch (Player->Gait)
	{
	case EGait::Walk:
		Player->SetGait(EGait::Walk);
		break;
	case EGait::Run:
		Player->SetGait(EGait::Run);
		break;
	}
}



#pragma endregion



#pragma region About Jump & Land
bool UPlayer_AnimInstance::JustLanded_Heavy()
{
	bool JustLanded = Player->bJustedLanded;

	float AbsLandVelocityZ = FMath::Abs(Player->LandVelocity.Z);
	float Threshold = FMath::Abs(HeavyLandSpeedThreshold);

	// �ж��Ƿ��� **�����**
	bool bJudgement1 = AbsLandVelocityZ >= Threshold;
	bJustLanded_Heavy = JustLanded && bJudgement1;

	return bJustLanded_Heavy;
}

bool UPlayer_AnimInstance::JustLanded_Light()
{
	bool JustLanded = Player->bJustedLanded;

	float AbsLandVelocityZ = FMath::Abs(Player->LandVelocity.Z);
	float Threshold = FMath::Abs(HeavyLandSpeedThreshold);

	// �ж��Ƿ��� **�����**
	bool bJudgement1 = AbsLandVelocityZ < Threshold;
	bJustLanded_Light = JustLanded && bJudgement1;

	return bJustLanded_Light;
}

#pragma endregion