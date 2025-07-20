// Copyright 2024

#include "GameSystems/CollectableSubsystem.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UCollectableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCollectableSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UCollectableSubsystem::AddToCollectedItems(FName Name, bool Value)
{
	CollectedItems.FindOrAdd(Name, Value);
	if(CollectedItems.Num() >= 8)
	{
		if(UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
		{
			DialogueSubsystem->AddCompletedEvent(AllCollectablesEvent);
		}
	}
}

void UCollectableSubsystem::SaveCollectables()
{
	/* if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveGameInstance->SavedCollectables = CollectedItems;

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	}*/

	if (USaveSubSystem* SSS = GetGameInstance()->GetSubsystem<USaveSubSystem>())
	{
		SSS->Save();
	}
}
