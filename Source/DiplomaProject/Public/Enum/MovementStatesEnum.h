#pragma once
#include "CoreMinimal.h"
#include "MovementStatesEnum.generated.h"


UENUM(BlueprintType)
enum class EMovementStates :uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Moving UMETA(DisplayName = "Moving")
};


