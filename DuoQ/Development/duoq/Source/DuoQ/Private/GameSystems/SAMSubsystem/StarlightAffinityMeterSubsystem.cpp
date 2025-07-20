// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/SAMSubsystem/StarlightAffinityMeterSubsystem.h"
#include "GameSystems/SaveSubSystem.h"

void UStarlightAffinityMeterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentSAMLevel = StartingSAMLevel;
}

void UStarlightAffinityMeterSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

FString UStarlightAffinityMeterSubsystem::PrintSAMUpdateOnScreen(bool Added)
{
	FString Output;
	if (Added)
	{
		Output = FString(TEXT("Starlight moved closer to you"));
	}
	else
	{
		Output = FString(TEXT("Starlight drifted further away from you"));
	}

	return Output;
}

FString UStarlightAffinityMeterSubsystem::PrintSAMInfoAsString()
{
	FString Output;

	// converting the current SAM level
	Output += FString::FromInt(1) + TEXT(", Current Starlight Affinity Level: ") + FString::FromInt(static_cast<uint8>(CurrentSAMLevel))
		+ TEXT(" ");
	// converting the maximum step size
	//Output += FString::FromInt(2) + TEXT(", Maximum amount the affinity can increase/decrease by: ") + FString::FromInt(MaxStepSize) + TEXT(" ");

	return Output;
}

void UStarlightAffinityMeterSubsystem::UpdateSAMScore(int ScoreChange)
{
	CurrentScore += ScoreChange;

	bool bLevelChanged = false;
	if(CurrentScore < 0 && CurrentSAMLevel != EStarlightAffectionLevel::Abysmal)
	{
		CurrentSAMLevel =  static_cast<EStarlightAffectionLevel>(static_cast<uint8>(CurrentSAMLevel) - 1);
		CurrentScore += ScoreToLevelUp[CurrentSAMLevel];
		bLevelChanged = true;
	}

	int* LevelUpReq = ScoreToLevelUp.Find(CurrentSAMLevel);
	if(LevelUpReq && CurrentScore >= *LevelUpReq && CurrentSAMLevel != EStarlightAffectionLevel::Good)
	{
		CurrentSAMLevel =  static_cast<EStarlightAffectionLevel>(static_cast<uint8>(CurrentSAMLevel) + 1);
		CurrentScore -= *LevelUpReq;
		bLevelChanged = true;
	}

	// I don't wanna reset player progress
	//GetGameInstance()->GetSubsystem<USaveSubSystem>()->Save(0, CurrentScore);

	OnSAMUpdate.Broadcast(ScoreChange,bLevelChanged);
}
