// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PoseSearch/PoseSearchTrajectoryTypes.h"

// Custom Enum
#include "Enum/MovementStatesEnum.h"
#include "Enum/GaitEnum.h"
#include "Enum/MovementPositionEnum.h"
#include "Enum/RotationModeEnum.h"


#include "PlayerCharacter.generated.h"

// input class forward declaration
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// UE5 Libaray
class UKismetSystemLibrary;

// UE5 Component forward declaration
class USpringArmComponent;
class UCameraComponent;
class APlayerController;
class USkeletalMeshComponent;

// Custom Component class forward declaration
class UPlayer_CMC;
class UPlayer_CTC;

// 创建监听事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementStateChanged, EMovementStates, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGaitChanged, EGait, NewGait);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementPositionChanged, EMovementPosition, NewPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRotationModeChanged, ERotationMode, NewRotationMode);


UCLASS()
class DIPLOMAPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyControllerChanged() override; 

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


// Delay struct parameter & Bind Function
public:
	/*-------------------------------CustomOnLandedEvent Delay-------------------------------*/
	FLatentActionInfo CustomOnLandedEvent_LatentInfo;

	UFUNCTION()
	void CustomOnLandedEvent_OnDelayFinished();

#pragma region Controller
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller")
	APlayerController* PlayerController;
#pragma endregion


#pragma region Input Variable & Function
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|MappingContext")
	UInputMappingContext* MainMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* MoveAction;
	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* LookAction;
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* SwitchMovementModeAction;
	void SwitchMovementMode();
	bool SwitchtoCharacterOrientation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* SwitchWalkorRunAction;
	void SwitchWalkorRun();

	
#pragma endregion


#pragma region UE5 Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;
#pragma endregion


#pragma region Custom Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Component")
	UPlayer_CMC* MovementComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory Component")
	//UPlayer_CTC* TrajectoryComponent;

#pragma endregion


#pragma region Custom Enum & Function
public:
	// Declaring event listeners
	UPROPERTY(BlueprintAssignable,Category = "Listener Event")
	FOnMovementStateChanged OnMovementStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Listener Event")
	FOnGaitChanged OnGaitChanged;

	UPROPERTY(BlueprintAssignable, Category = "Listener Event")
	FOnMovementPositionChanged OnMovementPositionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Listener Event")
	FOnRotationModeChanged OnRotationModeChanged;

	// 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Enum")
	EMovementStates MovementStates = EMovementStates::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Enum")
	EGait Gait = EGait::Walk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Enum")
	EMovementPosition MovementMode = EMovementPosition::OnGround;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Enum")
	ERotationMode RotationMode = ERotationMode::Strafe;


	// MovementStates, Gait, MovementPosition Get Function
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Custom Enum|Function")
	EMovementStates GetMovementStates();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Custom Enum|Function")
	EGait GetGait();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Custom Enum|Function")
	EMovementPosition GetMovementPosition();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Custom Enum|Function")
	ERotationMode GetRotationMode();

	// trigger a listener Event
	UFUNCTION(BlueprintCallable, Category = "Custom Enum|Function")
	void SetMovementStates(EMovementStates NewMovementStates);

	UFUNCTION(BlueprintCallable, Category = "Custom Enum|Function")
	void SetGait(EGait NewGait);

	UFUNCTION(BlueprintCallable, Category = "Custom Enum|Function")
	void SetMovementPosition(EMovementPosition NewMovementPosition);

	UFUNCTION(BlueprintCallable, Category = "Custom Enum|Function")
	void SetRotationMode(ERotationMode NewRotationMode);
#pragma endregion


#pragma region Necessray Value
public:
	// Spawn and draw Line trace to Gorund
	UFUNCTION(BlueprintCallable, Category = "Necessray Value")
	void TraceToGround();

	UFUNCTION(BlueprintCallable, Category = "Necessray Value")
	float CalculateTimeToLand(FVector StartLocation, FHitResult LineTraceHitResult);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Necessray Value")
	float TimeToLand;
#pragma endregion


#pragma region About Jump & Land
public:
	// 重写Character中的Landed
	 virtual void Landed(const FHitResult& Hit) override;

	// Custom Event - Character Movement Updated EVent
	UFUNCTION(BlueprintCallable, Category = "ABout Jump & Land")
	void CharacterMovementUpdatedEvent(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);

	UFUNCTION(BlueprintCallable, Category = "About Jump & Land")
	void CustomOnLandedEvent(FVector OldVelocity);

	UFUNCTION(BlueprintCallable, Category = "About Jump & Land")
	void UpdatedMovementSimulated(FVector OldVelocity);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "About Jump & Land")
	bool bJustedLanded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "About Jump & Land")
	FVector LandVelocity;
#pragma endregion


#pragma region Debug
public:
	UFUNCTION(BlueprintCallable, Category = "Custonm Draw Debug")
	void DrawCharacterVectorDirection();

	UFUNCTION(BlueprintCallable, Category = "Custonm Draw Debug")
	void DrawCameraVectorDirection();

#pragma endregion

};
