#pragma once
#include "CoreMinimal.h"
#include "GaitEnum.generated.h"


UENUM(BlueprintType)
enum class EGait :uint8
{
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run")
};
