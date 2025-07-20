// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/SaveSubSystem.h"

#include "DuoQGameUserSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/CollectableSubsystem.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "DuoQBlueprintFunctionLibrary.h"

void USaveSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
#if WITH_EDITOR
	NewSave();
#endif // WITH_EDITOR

	UE_LOG(LogTemp, Warning, TEXT("Initialize Save System"));

	// Load settings
	if (UGameUserSettings* GameUserSettings = UDuoQGameUserSettings::GetDuoQGameUserSettings())
	{
		GameUserSettings->LoadSettings();
		GameUserSettings->ApplySettings(false);
	}
}
void USaveSubSystem::NewSave()
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::CreateSaveGameObject(UDuoQSaveSystem::StaticClass())))
	{
		// Don't reset volume settings on new save
		if (UDuoQSaveSystem* OldSaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
		{
			SaveGameInstance->MusicVolume = OldSaveGameInstance->MusicVolume;
			SaveGameInstance->MasterVolume = OldSaveGameInstance->MasterVolume;
			SaveGameInstance->ChatVolume = OldSaveGameInstance->ChatVolume;
			SaveGameInstance->SFXVolume = OldSaveGameInstance->SFXVolume;
		}
		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	}
}

void USaveSubSystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize Save System"));
}
void USaveSubSystem::Save(int PlayerProgress, int StarlightAffinity)
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		// Actually put in the data into SaveGameInstance
		if (PlayerProgress > SaveGameInstance->PlayerProgress)
		{
			SaveGameInstance->PlayerProgress = PlayerProgress;
			SaveGameInstance->StarlightAffinity = StarlightAffinity;
			SaveGun(bPlayerHasGun,bTalaHasGun);
		}

		// add this actor to the list of Collectables to Collectable Subsystem
		if (UDuoQGameInstance* DGI = UDuoQBlueprintFunctionLibrary::GetDuoQGameInstance(this))
		{
			// trying to grab the collectable subsystem
			if (UCollectableSubsystem* CS = DGI->GetSubsystem<UCollectableSubsystem>())
			{
				SaveGameInstance->SavedCollectables = CS->CollectedItems;
			}

			if (ADuoQPlayerCharacter* Player = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(GetWorld()))
			{
				SaveGameInstance->SavedEncounter = Player->CurrentEncounter;
			}

			// getting the dialogue subsystem
			if (UDialogueSubsystem* DS = DGI->GetSubsystem<UDialogueSubsystem>())
			{
				if (PlayerProgress > SaveGameInstance->PlayerProgress)
				{
					SaveGameInstance->SavedTriggeredDialogue = DS->GetAllEventsTriggered();
					TriggeredEventToLoad = DS->GetAllEventsTriggered();
				}
			}
		}

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	}
}
void USaveSubSystem::SaveGun(bool PlayerHasGun, bool StarlightHasGun)
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveGameInstance->PlayerHasGun = PlayerHasGun || SaveGameInstance->PlayerHasGun;
		SaveGameInstance->StarlightHasGun = StarlightHasGun || SaveGameInstance->StarlightHasGun;

		UE_LOG(LogTemp, Display, TEXT("Player has %s, and Starlight has %s"), SaveGameInstance->PlayerHasGun ? TEXT("true") : TEXT("false"), SaveGameInstance->StarlightHasGun ? TEXT("true") : TEXT("false"));
		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	}
}

void USaveSubSystem::SaveMenuSettings(float Master, float Music, float Chat, float SFX)
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveGameInstance->MusicVolume = Music;
		SaveGameInstance->MasterVolume = Master;
		SaveGameInstance->ChatVolume = Chat;
		SaveGameInstance->SFXVolume = SFX;
		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	}
}

void USaveSubSystem::SetGun(bool PlayerGun, bool StarlightGun)
{
	bPlayerHasGun = PlayerGun || bPlayerHasGun;
	bTalaHasGun = StarlightGun || bTalaHasGun;
}

void USaveSubSystem::SaveEnemy(FName enemy, bool status)
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveGameInstance->EnemyStatus[enemy] = status;
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	}
}
int USaveSubSystem::DebugBackward()
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveGameInstance->PlayerProgress -= 1;
		if (SaveGameInstance->PlayerProgress < 0)
		{
			SaveGameInstance->PlayerProgress = 0;
		}

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
		return SaveGameInstance->PlayerProgress;
	}
	return -1;
}

int USaveSubSystem::DebugToLevel(int TargetProgress)
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveGameInstance->PlayerProgress = TargetProgress;
		if (SaveGameInstance->PlayerProgress < 0)
		{
			SaveGameInstance->PlayerProgress = 0;
		}

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
		return SaveGameInstance->PlayerProgress;
	}
	return 0;
}

int USaveSubSystem::DebugForward()
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveGameInstance->PlayerProgress += 1;

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
		return SaveGameInstance->PlayerProgress;
	}
	return -1;
}

int USaveSubSystem::GetPlayerProgress()
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		return (SaveGameInstance->PlayerProgress);
	}
	return -1;
}

int USaveSubSystem::GetStarlightAffinity()
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		return (SaveGameInstance->StarlightAffinity);
	}
	return 0;
}
bool USaveSubSystem::GetStarlightHasGun()
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		return (SaveGameInstance->StarlightHasGun);
	}
	return 0;
}
bool USaveSubSystem::GetPlayerHasGun()
{
	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		return (SaveGameInstance->PlayerHasGun);
	}
	return 0;
}