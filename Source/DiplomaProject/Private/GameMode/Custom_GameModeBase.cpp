// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Custom_GameModeBase.h"
#include "Character/PlayerCharacter/MyCharacter.h"
#include "UObject/ConstructorHelpers.h"


ACustom_GameModeBase::ACustom_GameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/BP_MyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
