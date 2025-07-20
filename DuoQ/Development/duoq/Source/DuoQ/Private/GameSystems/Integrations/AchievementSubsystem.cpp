// Copyright 2024


#include "GameSystems/Integrations/AchievementSubsystem.h"

#include "eos_achievements.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UAchievementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UAchievementSubsystem::CheckAchievementUnlock(FString Event)
{
	if(auto Achievement = AchievementsList.Find(Event))
	{
		UnlockAchievement(*Achievement);
	}
}

void UAchievementSubsystem::SetUpAchievements()
{
	if(UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->OnEventRaised.RemoveAll(this);
		DialogueSubsystem->OnEventRaised.AddDynamic(this,&UAchievementSubsystem::CheckAchievementUnlock);
	}
}

APlayerController* UAchievementSubsystem::GetPlayerController()
{
	return UGameplayStatics::GetPlayerController(this,0);
}

void UAchievementSubsystem::UnlockAchievement(FString Achievement)
{
	//EOS_Achievements_UnlockAchievementsOptions();
	//EOS_Achievements_UnlockAchievements()
	UnlockSteamAchievement(FName(Achievement));
}