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
	// ��ʼ��Trajectory����ȡ��ر���
	UFUNCTION(BlueprintCallable, Category = "About Trajectory")
	void GenerateTrajectory(float HistorySamplingInterval = 0.04f, int HistorySamplingCount = 10,
							float PredictionSamplingInterval = 0.2f, int PredictionSamplingCount = 8,
							float DebugThickness = 1.f, float HeightOffset = 0.f);

	// �洢��ǰ֡��ʱ������
	float DeltaTime;

	// �켣���ݱ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	FPoseSearchTrajectoryData TrajectoryGenerateDataMoving;

	// ��������Ĺ켣����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	FPoseSearchQueryTrajectory InOutTrajectory;

	// ��PoseSearchGenerateTrajectory�������ɵĹ켣���ݣ���������ʷ��Ԥ��켣��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	FPoseSearchQueryTrajectory Trajectory;

	// ��¼��һ�θ��µĿ�������ת�Ƕ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	float InOutDesiredControllerYawLastUpdate;

	// ����δ���ٶȣ������жϽ�ɫ״̬(��: �Ƿ�ʼ�ƶ�)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "About Trajectory")
	FVector vFutureVelocity;





	 
	/*-------------------------------------Update Necessary Variable -------------------------------------------*/
	// ��Tick��ʵʱ���±�Ҫ����
	UFUNCTION(BlueprintCallable, Category = "Necessray Variable")
	void UpdateNecessaryVariables();

	// �ж�Character�Ƿ����ƶ�
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Necessary Variable")
	bool IsMoving();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	bool bIsMoving = false;

	// �ж�Character�Ǹտ�ʼ�ƶ������Ѿ������ƶ�״̬��
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Necessary Variable")
	bool IsStarting();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	bool bIsStarting = false;

	// Character�Ķ�ά�ٶ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	float fSpeed2D;

	// Character�ĵ�ǰ�ٶ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	FVector vVelocity;

	// Character�Ƿ��ڿ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessary Variable")
	bool bIsFalling;


	/*-------------------------------------Update State-------------------------------------------*/
	// UpdateState(): ��Tick��ʵʱ���¹ؼ�Enum�ı���
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
