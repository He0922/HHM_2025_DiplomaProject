// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTrajectoryComponent.h"
#include "PoseSearch/PoseSearchTrajectoryTypes.h"
#include "Player_CTC.generated.h"

UCLASS()
class DIPLOMAPROJECT_API UPlayer_CTC : public UCharacterTrajectoryComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Player Trajectory Component")
	const FPoseSearchQueryTrajectory& GetTrajectory_Player_CTC()const;

};
