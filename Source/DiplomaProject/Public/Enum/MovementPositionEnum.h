#pragma once
#include "CoreMinimal.h"
#include "MovementPositionEnum.generated.h"


UENUM(BlueprintType)
enum class EMovementPosition :uint8
{
	OnGround UMETA(DisplayName = "OnGround"),
	InAir UMETA(DisplayName = "InAir")
};


