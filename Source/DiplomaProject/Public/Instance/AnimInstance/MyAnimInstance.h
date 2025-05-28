// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "Enum/GaitEnum.h"
#include "Enum/MovementStatesEnum.h"
#include "Enum/MovementPositionEnum.h"
#include "Enum/RotationModeEnum.h"
#include "Struct/CharacterInputStateStruct.h"

// Pose Search Library
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"

#include "MyAnimInstance.generated.h"


class AMyCharacter;
class UPlayer_CMC;

/**
 * 
 */
UCLASS()
class DIPLOMAPROJECT_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Character")
	AMyCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My Movement Component")
	UPlayer_CMC* MovementComponent;


/*===================================== Struct & Enum =====================================*/
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My States")
	EMovementStates MovementState = EMovementStates::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My States")
	EGait Gait = EGait::Run;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My States")
	EMovementPosition MovementPosition = EMovementPosition::OnGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My States")
	ERotationMode RotationMode = ERotationMode::Strafe;


/*===================================== Listener Event =====================================*/
protected:
	UFUNCTION()
	void MovementStateChanged(EMovementStates NewMovementState);

	UFUNCTION()
	void GaitChanged(EGait NewGait);

	UFUNCTION()
	void MovementPositionChanged(EMovementPosition NewMovementPosition);

	UFUNCTION()
	void RotationModeChanged(ERotationMode NewRotationMode);


/*================================== Generate Trajectory ===================================*/
protected:
	UFUNCTION(BlueprintCallable, Category = "My Generate Trajectory")
	void GenerateTrajectory(float HistorySamplingInterval = 0.04f, int HistorySamplingCount = 10,
							float PredictionSamplingInterval = 0.2f, int PredictionSamplingCount = 8,
							float DebugThickness = 1.f, float HeightOffset = 0.f);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Generate Trajectory")
	FPoseSearchTrajectoryData InTrajectoryData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Generate Trajectory")
	float DeltaTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Generate Trajectory")
	FPoseSearchQueryTrajectory InOutTrajectory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Generate Trajectory")
	float InOutDesiredControllerYawLastUpdate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Generate Trajectory")
	FPoseSearchQueryTrajectory Trajectory;


/*================================== Necessary Variable ===================================*/
protected:
	UFUNCTION(BlueprintCallable, Category = "My Necessary Variable")
	void UpdateNecessaryVariable();

	// Velocity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Necessary Variable")
	FVector vVelocity;

	// FutureVelocity
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "My Necessary Variable")
	FVector GetFutureVelocity(float FutureTime = 0.5f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Necessary Variable")
	FVector vFutureVelocity;

	// Speed 2D
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Necessary Variable")
	float fSpeed2D;


	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "My Necessary Variable")
	bool IsStarting(float StartThreshold = 10.f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Necessary Variable")
	bool bIsStarting;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "My Necessary Variable")
	bool IsMoving();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Necessary Variable")
	bool bIsMoving;


	UFUNCTION(BlueprintCallable, Category = "My Necessary Variable")
	void IsPivoting();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Necessary Variable")
	bool bIsPivoting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Necessary Variable")
	float Time;

/*==================================== Update State =====================================*/
protected:
	UFUNCTION(BlueprintCallable, Category = "Update Necessary State")
	void UpdateState();


	UFUNCTION(BlueprintCallable, Category = "Update Necessary State")
	void UpdateMovementState();

	UFUNCTION(BlueprintCallable, Category = "Update Necessary State")
	void UpdateGait();

	UFUNCTION(BlueprintCallable, Category = "Update Necessary State")
	void UpdateMovementPosition();

	UFUNCTION(BlueprintCallable, Category = "Update Necessary State")
	void UpdateRotationMode();



};
