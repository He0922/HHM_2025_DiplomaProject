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
// 生成轨迹数据，并计算未来速度
void UPlayer_AnimInstance::GenerateTrajectory(float HistorySamplingInterval, int HistorySamplingCount,
												float PredictionSamplingInterval, int PredictionSamplingCount,
												float DebugThickness, float HeightOffset)
{
	APlayerCharacter* OwningCharacter = Cast<APlayerCharacter>(GetOwningActor());
	if (!OwningCharacter) return;

	DeltaTime = GetWorld()->GetDeltaSeconds();

	InOutDesiredControllerYawLastUpdate = OwningCharacter->GetControlRotation().Yaw;

	// 使用PoseSearchGenerateTrajectory函数生成轨迹数据
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

	

	// Trajectory中未来的0.5s和0.4s位置计算出未来速度(基于轨迹点的位移变化)
	/*
	为什么不选择更大的时间间隔: 
	1. 响应速度变慢: 如果时间间隔过长，FutureVelocity可能无法及时反映角色的加速度或减速状态
	2. 不适用于高频动画匹配: 如果游戏的动画更新频率较高, 长时间间隔可能导致动画切换滞后
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
	// 若未来速度大于当前速度至少100，说明Character还在加速，如果未来速度小与(当前速度+100)，说明Character已经进入了匀速运动
	// FutureVelocity正常情况下不会大于MaxWalkSpeed，因为FutureVelocity是PoseSearchGenerateTrajectory生成的，
	// 它通常是遵循CharacterMovementComponent的速度限制
	bool Judgment1 = vFutureVelocity.Size2D() >= (vVelocity.Size2D() + 100.f);
	bool Judgment2 = IsMoving();

	bIsStarting = Judgment1 && Judgment2;

	return bIsStarting;
}


/*-------------------------------------Update State-------------------------------------------*/
void UPlayer_AnimInstance::UpdateState()
{
	// 通过MovementComponent中的MovementMode进行自定义的MovementPosition枚举变量改变，
	// 无需再Character类中绑定空格按键以改变枚举变量
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

	// 判断是否是 **重落地**
	bool bJudgement1 = AbsLandVelocityZ >= Threshold;
	bJustLanded_Heavy = JustLanded && bJudgement1;

	return bJustLanded_Heavy;
}

bool UPlayer_AnimInstance::JustLanded_Light()
{
	bool JustLanded = Player->bJustedLanded;

	float AbsLandVelocityZ = FMath::Abs(Player->LandVelocity.Z);
	float Threshold = FMath::Abs(HeavyLandSpeedThreshold);

	// 判断是否是 **轻落地**
	bool bJudgement1 = AbsLandVelocityZ < Threshold;
	bJustLanded_Light = JustLanded && bJudgement1;

	return bJustLanded_Light;
}

#pragma endregion