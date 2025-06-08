// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter/MyCharacter.h"

// Component
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "Controller/MainController.h"

// Library
#include "Kismet/KismetSystemLibrary.h"

// My Components
#include "Component/MovementComponent/Player_CMC.h"

// My Misc
#include "../../../DebugHelper.h"


// Sets default values
AMyCharacter::AMyCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayer_CMC>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;


	// Creat Movement Component
	MovementComponent = Cast<UPlayer_CMC>(GetMovementComponent());
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->bUseControllerDesiredRotation = true;
	MovementComponent->MaxWalkSpeed = 500.f;


	// Creat Spring Arm Component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;


	// Create Camera Component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CustomOnLandedEvent_LatentInfo.Linkage = 0;
	CustomOnLandedEvent_LatentInfo.UUID = GetUniqueID();
	CustomOnLandedEvent_LatentInfo.ExecutionFunction = TEXT("CustomLandedEvent_OnDelayFinished");
	CustomOnLandedEvent_LatentInfo.CallbackTarget = this;

	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DrawCharacterVectorDirection();
	//DrawCameraVectorDirection();
	//EnumDebug();
	VariablesDebug();
}


void AMyCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	PlayerController = Cast<AMainController>(GetController());
	
	if(PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			EnhancedInputLocalSubSystem->AddMappingContext(MainMappingContext, 0);
		}
	}
}


// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(SwitchRotationModeAction, ETriggerEvent::Started, this, &AMyCharacter::SwitchRotationMode);
		EnhancedInputComponent->BindAction(SwitchGaitAction, ETriggerEvent::Started, this, &AMyCharacter::SwitchGait);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
	}
}


void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	FVector CurrentVelocity = GetVelocity();

	SetLandVelocity(CurrentVelocity);
	SetIsJustLanded(true);

	UKismetSystemLibrary::Delay(GetWorld(), 1.f, CustomOnLandedEvent_LatentInfo);
}


/*-------------------------------CustomOnLandedEvent Delay-------------------------------*/
void AMyCharacter::CustomLandedEvent_OnDelayFinished()
{
	SetIsJustLanded(false);
}


/*========================================= Input =========================================*/
void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (PlayerController != nullptr)
	{
		const FRotator Rotation = PlayerController->GetControlRotation();
		const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}


void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (PlayerController != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void AMyCharacter::SwitchGait()
{
	MovementComponent->MaxWalkSpeed = (Gait == EGait::Walk) ? 500.f : 300.f;
	EGait NewGait = (CharacterInputState.ToggleGait()) ? EGait::Walk : EGait::Run;
	SetGait(NewGait);
}


void AMyCharacter::SwitchRotationMode()
{
	MovementComponent->bOrientRotationToMovement = MovementComponent->bOrientRotationToMovement ? false : true;
	MovementComponent->bUseControllerDesiredRotation = MovementComponent->bUseControllerDesiredRotation ? true : false;
	ERotationMode NewRotationMode = (CharacterInputState.ToggleRotationMode()) ? ERotationMode::Strafe : ERotationMode::OrientToMovement;
	SetRotationMode(NewRotationMode);
}


/*===================================== Struct & Enum =====================================*/
void AMyCharacter::SetMovementState(EMovementStates NewMovementState)
{
	MovementState = NewMovementState;
	MovementStateChanged.Broadcast(NewMovementState);
}


void AMyCharacter::SetGait(EGait NewGait)
{
	Gait = NewGait;
	GaitChanged.Broadcast(NewGait);
}


void AMyCharacter::SetMovementPosition(EMovementPosition NewMovementPosition)
{
	MovementPosition = NewMovementPosition;
	MovementPositionChanged.Broadcast(NewMovementPosition);
}


void AMyCharacter::SetRotationMode(ERotationMode NewRotationMode)
{
	RotationMode = NewRotationMode;
	RotationModeChanged.Broadcast(NewRotationMode);
}




void AMyCharacter::SetLandVelocity(FVector NewLandVelocity)
{
	vLandVelocity = NewLandVelocity;
	LandVelocityChanged.Broadcast(NewLandVelocity);
}

void AMyCharacter::SetIsJustLanded(bool NewIsJustLanded)
{ 
	bIsJustLanded = NewIsJustLanded;
	IsLandedChanged.Broadcast(NewIsJustLanded);
}

/*======================================= Debug Draw =======================================*/
void AMyCharacter::DrawCharacterVectorDirection()
{
	// Get Vector for Character Move Direction
	FVector Velocity = GetVelocity();
	// Debug::PrintVector("Character Velocity: ", Velocity, 0.f, false, FColor::Green);

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


void AMyCharacter::DrawCameraVectorDirection()
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


void AMyCharacter::EnumDebug()
{

}


void AMyCharacter::VariablesDebug()
{

}
