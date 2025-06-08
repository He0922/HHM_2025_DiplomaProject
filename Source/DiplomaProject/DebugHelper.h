#pragma once
#include "Engine/Engine.h"
#include "UObject/NameTypes.h"
#include "UObject/UnrealType.h"



// Ê±¼ä´Á




namespace Debug
{
    // Basic Data Types
	static void Print(const FString& Msg,const float Time, bool PrintLog, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, Time, Color, Msg);
		}

		if (PrintLog) { UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg); }
	}

	static void PrintBool(const FString& Msg, bool bValue, const float Time, bool PrintLog, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
	{
		FString BoolString = bValue ? TEXT("true") : TEXT("false");
		FString FullMsg = Msg + BoolString;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, Time, Color, FullMsg);
		}

		if (PrintLog) {UE_LOG(LogTemp, Warning, TEXT("%s"), *FullMsg);}
		
	}


    static void PrintFloat(const FString& Msg, float Value, float Time, bool PrintLog, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
    {
        FString FullMsg = Msg + FString::SanitizeFloat(Value);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(InKey, Time, Color, FullMsg);
        }

		if (PrintLog) { UE_LOG(LogTemp, Warning, TEXT("%s"), *FullMsg); }
    }

    static void PrintVector(const FString& Msg, const FVector& Value, float Time, bool PrintLog, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
    {
        FString FullMsg = Msg + Value.ToString();

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(InKey, Time, Color, FullMsg);
        }

		if (PrintLog) { UE_LOG(LogTemp, Warning, TEXT("%s"), *FullMsg); }
    }

    static void PrintInt(const FString& Msg, int32 Value, float Time, bool PrintLog, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
    {
        FString FullMsg = Msg + FString::FromInt(Value);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(InKey, Time, Color, FullMsg);
        }

        if (PrintLog) { UE_LOG(LogTemp, Warning, TEXT("%s"), *FullMsg); }
    }


    // Complex Data Types
    template<typename EnumType>
    static void PrintEnum(const FString& Msg, EnumType Value, float Time, bool PrintLog, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
    {
        const UEnum* EnumPtr = StaticEnum<EnumType>();
        FString EnumString;
        if (EnumPtr)
        {
            EnumString = EnumPtr->GetNameStringByValue(static_cast<int64>(Value));
        }
        else
        {
            EnumString = TEXT("Invalid Enum");
        }

        FString FullMsg = Msg + EnumString;

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(InKey, Time, Color, FullMsg);
        }

        if (PrintLog) { UE_LOG(LogTemp, Warning, TEXT("%s"), *FullMsg); }
    }

	static void PrintHitResult(const FHitResult& HitResult, bool PrintLog, const float Time, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
	{
		FString HitResultString = FString::Printf(TEXT("HitResult - Actor: %s, Location: %s, Impact Point: %s, Normal: %s"),
			*HitResult.GetActor()->GetName(),
			*HitResult.Location.ToString(),
			*HitResult.ImpactPoint.ToString(),
			*HitResult.ImpactNormal.ToString());

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, Time, Color, HitResultString);
		}

		if (PrintLog) { UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResultString); }
	}
}