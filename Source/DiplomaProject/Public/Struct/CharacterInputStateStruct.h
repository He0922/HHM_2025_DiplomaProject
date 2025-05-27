#pragma once
#include "CoreMinimal.h"
#include "CharacterInputStateStruct.generated.h"


USTRUCT(BlueprintType)
struct FCharacterInputState
{
	GENERATED_BODY()

public:
	FCharacterInputState()
	{
		bShouldWalk = false;
		bShouldStrafe = true;
	}


	bool ToggleGait() 
	{ 
		bShouldWalk = !bShouldWalk;
		return bShouldWalk;
	}

	bool ToggleRotationMode() 
	{ 
		bShouldStrafe = !bShouldStrafe;
		return bShouldStrafe;
	}



public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Input State")
	bool bShouldWalk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Input State")
	bool bShouldStrafe;

};