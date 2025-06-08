#pragma once
#include "CoreMinimal.h"
#include "LandedTypeEnum.generated.h"


UENUM(BlueprintType)
enum class ELandedType :uint8
{
	None UMETA(DisplayName = "None"),
	Light UMETA(DisplayName = "Light"),
	Heavy UMETA(DisplayName = "Heavy")
};

