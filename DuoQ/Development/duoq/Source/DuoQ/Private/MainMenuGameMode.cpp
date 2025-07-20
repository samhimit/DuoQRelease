// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "GameSystems/Audio/AudioSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystems/DuoQSaveSystem.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
	{
		if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
		{
			AudioSubsystem->SetMasterVolume(SaveGameInstance->MasterVolume);
			AudioSubsystem->SetMusicVolume(SaveGameInstance->MusicVolume);
			AudioSubsystem->SetSfxVolume(SaveGameInstance->SFXVolume);
			AudioSubsystem->SetVoiceVolume(SaveGameInstance->ChatVolume);
		}
		else
		{
			AudioSubsystem->SetMasterVolume(0.5f);
			AudioSubsystem->SetMusicVolume(0.5f);
			AudioSubsystem->SetSfxVolume(0.5f);
			AudioSubsystem->SetVoiceVolume(0.5f);
		}
	}
}
