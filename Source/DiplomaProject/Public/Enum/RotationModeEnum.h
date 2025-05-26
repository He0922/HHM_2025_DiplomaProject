#pragma once
#include "CoreMinimal.h"
#include "RotationModeEnum.generated.h"


UENUM(BlueprintType)
enum class ERotationMode :uint8
{
	OrientToMovement UMETA(DisplayName = "OrientToMovement"), // Movement based on character orientation
	Strafe UMETA(DisplayName = "Strafe") // Movement based on Camera orientation
};
