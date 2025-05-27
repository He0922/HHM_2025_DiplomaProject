// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// My Enum & Struct
#include "Enum/GaitEnum.h"
#include "Enum/MovementStatesEnum.h"
#include "Enum/MovementPositionEnum.h"
#include "Enum/RotationModeEnum.h"
#include "Struct/CharacterInputStateStruct.h"

#include "MyCharacter.generated.h"


// Input class
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// Component
class USpringArmComponent;
class UCameraComponent;

// My Component
class UPlayer_CMC;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovementStateChanged, EMovementStates, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGaitChanged, EGait, NewGait);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovementPositionChanged, EMovementPosition, NewPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRotationModeChanged, ERotationMode, NewRotationMode);


UCLASS()
class DIPLOMAPROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;


protected:
	// Player Controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	APlayerController* PlayerController;

	// SpringArm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Component")
	USpringArmComponent* SpringArm;

	// Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Component")
	UCameraComponent* Camera;

public:
	// Movement Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Component")
	UPlayer_CMC* MovementComponent;


/*========================================= Input =========================================*/
protected:
	// Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MainMappingContext;

	// Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* MoveAction;
	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* LookAction;
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* SwitchRotationModeAction;
	void SwitchRotationMode();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	UInputAction* SwitchGaitAction;
	void SwitchGait();

	
/*===================================== Struct & Enum =====================================*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My States")
	FCharacterInputState CharacterInputState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My States")
	EMovementStates MovementState = EMovementStates::Idle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My States")
	EGait Gait = EGait::Run;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My States")
	EMovementPosition MovementPosition = EMovementPosition::OnGround;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My States")
	ERotationMode RotationMode = ERotationMode::Strafe;


	// Set Enums
	UFUNCTION(BlueprintCallable, Category = "My States|Function")
	void SetMovementState(EMovementStates NewMovementStates);

	UFUNCTION(BlueprintCallable, Category = "My States|Function")
	void SetGait(EGait NewGait);

	UFUNCTION(BlueprintCallable, Category = "My States|Function")
	void SetMovementPosition(EMovementPosition NewMovementPosition);

	UFUNCTION(BlueprintCallable, Category = "My States|Function")
	void SetRotationMode(ERotationMode NewRotationMode);


	// ¼àÌýÊÂ¼þ
	UPROPERTY(BlueprintAssignable, Category = "My States|Listener Event")
	FMovementStateChanged MovementStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "My States|Listener Event")
	FGaitChanged GaitChanged;

	UPROPERTY(BlueprintAssignable, Category = "My States|Listener Event")
	FMovementPositionChanged MovementPositionChanged;

	UPROPERTY(BlueprintAssignable, Category = "My States|Listener Event")
	FRotationModeChanged RotationModeChanged;


/*======================================= Debug Draw =======================================*/
public:
	void DrawCharacterVectorDirection();

	void DrawCameraVectorDirection();
};
