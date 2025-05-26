// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

// Custom Enum
#include "Enum/MovementStatesEnum.h"
#include "Enum/GaitEnum.h"
#include "Enum/MovementPositionEnum.h"
#include "Enum/RotationModeEnum.h"


#include "PoseSearch/PoseSearchTrajectoryLibrary.h"

#include "Player_AnimInstance.generated.h"

class APlayerCharacter;
class UPlayer_CTC;
class UPlayer_CMC;



UCLASS()
class DIPLOMAPROJECT_API UPlayer_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Variable")
	APlayerCharacter* Player;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Variable")
	// UPlayer_CTC* Trajectory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Variable")
	UPlayer_CMC* MovementComponent;


#pragma region Necessity Variable & Function
public:

#pragma endregion


#pragma region Custom Enum & Function
public:
	// Subscribe to Listener events in PlayerCharacter
	UFUNCTION()
	void OnMovementStateChanged(EMovementStates NewMovementState);

	UFUNCTION()
	void OnGaitChanged(EGait NewGait);

	UFUNCTION()
	void OnMovementPositionChanged(EMovementPosition NewMovementMode);

	UFUNCTION()
	void OnRotationModeChanged(ERotationMode NewRotationMode);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Enum")
	EMovementStates MovementState = EMovementStates::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Enum")
	EGait Gait = EGait::Walk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Enum")
	EMovementPosition MovementMode = EMovementPosition::OnGround;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Enum")
	ERotationMode RotationMode = ERotationMode::Strafe;
#pragma endregion


#pragma region Necessary Variable & Function
public:
	/*-------------------------------------Generate Trajectory -------------------------------------------*/
	// 初始化Trajectory并获取相关变量
	UFUNCTION(BlueprintCallable, Category = "About Trajectory")
	void GenerateTrajectory(float HistorySamplingInterval = 0.04f, int HistorySamplingCount = 10,
							float PredictionSamplingInterval = 0.2f, int PredictionSamplingCount = 8,
							float DebugThickness = 1.f, float HeightOffset = 0.f);

	// 存储当前帧的时间增量
	float DeltaTime;

	// 轨迹数据变量
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	FPoseSearchTrajectoryData TrajectoryGenerateDataMoving;

	// 输入输出的轨迹数据
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	FPoseSearchQueryTrajectory InOutTrajectory;

	// 由PoseSearchGenerateTrajectory函数生成的轨迹数据，包含了历史和预测轨迹点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	FPoseSearchQueryTrajectory Trajectory;

	// 记录上一次更新的控制器旋转角度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	float InOutDesiredControllerYawLastUpdate;

	// 计算未来速度，用于判断角色状态(如: 是否开始移动)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	FVector vFutureVelocity;





	 
	/*-------------------------------------Update Necessary Variable -------------------------------------------*/
	// 在Tick中实时更新必要变量
	UFUNCTION(BlueprintCallable, Category = "Necessray Variable")
	void UpdateNecessaryVariables();

	// 判断Character是否在移动
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Necessary Variable")
	bool IsMoving();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	bool bIsMoving = false;

	// 判断Character是刚开始移动还是已经处于移动状态中
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Necessary Variable")
	bool IsStarting();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	bool bIsStarting = false;

	// Character的二维速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	float fSpeed2D;

	// Character的当前速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	FVector vVelocity;

	// Character是否处于空中
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	bool bIsFalling;


	/*-------------------------------------Update State-------------------------------------------*/
	// UpdateState(): 在Tick中实时更新关键Enum的变量
	UFUNCTION(BlueprintCallable, Category = "Update State")
	void UpdateState();


#pragma endregion


#pragma region About Jump & Land
public:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "About Jump & Land")
	bool JustLanded_Heavy();
	
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "About Jump & Land")
	bool JustLanded_Light();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "About Jump & Land")
	bool bJustLanded_Heavy = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "About Jump & Land")
	bool bJustLanded_Light = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "About Jump & Land")
	float HeavyLandSpeedThreshold = 700.f;


#pragma endregion

};
