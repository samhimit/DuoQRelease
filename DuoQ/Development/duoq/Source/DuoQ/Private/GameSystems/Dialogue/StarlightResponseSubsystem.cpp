// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/Dialogue/StarlightResponseSubsystem.h"

#include "AudioDevice.h"
#include "UI/DialogueWidget.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "MainMenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "DuoQ/Public/GameSystems/DuoQGameMode.h"
#include "DuoQ/Public/Characters/Partner/StarlightCharacter.h"
#include "Components/AudioComponent.h"
#include "GameSystems/Communication/CommunicationSubsystem.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "UI/DuoQHUD.h"
#include "UI/IntroSequenceHUD.h"

/*
void UStarlightResponseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	VoiceLines = TMap<FString, TObjectPtr<USoundBase>>();

	TArray<FAssetData> voAssets;
	IAssetRegistry::Get()->GetAssetsByPath(FName(TEXT("/Game/Sounds/VO")), voAssets);
	for (FAssetData asset : voAssets)
	{
		if (USoundBase* soundAsset = Cast<USoundBase>(asset.GetAsset()))
		{
			FString soundName = soundAsset->GetName();
			VoiceLines.Add(soundName, soundAsset);
		}
	}
}*/

void UStarlightResponseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UStarlightResponseSubsystem::HandleCallout(USoundBase* VoiceOver, FString Subtitle)
{
	SetStarlightTalking(true);

	if (AStarlightCharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this); StarlightCharacter && VoiceOver)
	{
		StarlightCharacter->VoiceSource->Sound = VoiceOver;
		StarlightCharacter->VoiceSource->Play();
		DialogueShowTime = VoiceOver->Duration;
		GetWorld()->GetTimerManager().SetTimer(DialogueTimer, this, &UStarlightResponseSubsystem::HideDialogue, DialogueShowTime);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(DialogueTimer, this, &UStarlightResponseSubsystem::HideDialogue, TempDialogueShowTime);
	}

	if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
	{
		DuoQPlayerCharacter->PlayerHUD->DialogueHUD->ShowSubtitle(Subtitle);
	}
	else if (AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		MainMenuGameMode->MenuHUD->DialogueHUD->ShowSubtitle(Subtitle);
	}
}

void UStarlightResponseSubsystem::HandleDialogue(FString& LineName, FString& Subtitle, USoundBase* VoiceOver)
{
	// Reset the Player Response timer bc the dialogue is continuing, meaning the player must have responded
	if (UStarlightResponseSubsystem* SRS = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
	{
		GetWorld()->GetTimerManager().ClearTimer(SRS->PlayerResponseTimer);
		UE_LOG(LogTemp, Log, TEXT("Player response timer cleared"));
	}

	SetStarlightTalking(true);

	if (bIsStarlight && !(LineName == "EE_Pink" || LineName == "EE_LetsGo" || LineName == "Lunch"))
	{
		if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
		{
			VoiceOver = CommunicationSubsystem->GOTOK;
		}
	}

	if (AStarlightCharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this); StarlightCharacter && VoiceOver)
	{
		StarlightCharacter->VoiceSource->Sound = VoiceOver;
		StarlightCharacter->VoiceSource->Play();
		DialogueShowTime = VoiceOver->Duration;
		GetWorld()->GetTimerManager().SetTimer(DialogueTimer, this, &UStarlightResponseSubsystem::HideDialogue, DialogueShowTime);
	}
	else if (VoiceOver)
	{
		if (VoiceOverSound)
		{
			VoiceOverSound->Stop();
		}
		VoiceOverSound = UGameplayStatics::CreateSound2D(this, VoiceOver);
		VoiceOverSound->Play();
		DialogueShowTime = VoiceOver->Duration;
		GetWorld()->GetTimerManager().SetTimer(DialogueTimer, this, &UStarlightResponseSubsystem::HideDialogue, DialogueShowTime);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(DialogueTimer, this, &UStarlightResponseSubsystem::HideDialogue, TempDialogueShowTime);
	}

	if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
	{
		if (DuoQPlayerCharacter->PlayerHUD != nullptr)
		{
			if (DuoQPlayerCharacter->PlayerHUD->DialogueHUD != nullptr)
			{
				DuoQPlayerCharacter->PlayerHUD->DialogueHUD->ShowSubtitle(Subtitle);
			}
		}
	}
	else if (AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (MainMenuGameMode->MenuHUD != nullptr)
		{
			if (MainMenuGameMode->MenuHUD->DialogueHUD != nullptr)
			{
				MainMenuGameMode->MenuHUD->DialogueHUD->ShowSubtitle(Subtitle);
			}
		}
	}
}

void UStarlightResponseSubsystem::HideDialogue()
{
	if (!bIsStarlightTalking)
	{
		return;
	}

	SetStarlightTalking(false);

	if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
	{
		// DuoQPlayerCharacter->PlayerHUD->DialogueHUD->HideSubtitle();
		GetWorld()->GetTimerManager().SetTimer(PlayerResponseTimer, this, &UStarlightResponseSubsystem::OnPlayerResponseTimeout, PlayerResponseTime);
	}
	else if (AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		// MainMenuGameMode->MenuHUD->DialogueHUD->HideSubtitle();
	}
}

void UStarlightResponseSubsystem::OnPlayerResponseTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("Player takes too long to respond"));
}

void UStarlightResponseSubsystem::SetStarlightTalking(bool bIsTalking)
{
	bIsStarlightTalking = bIsTalking;

	OnStarlightTalkingChanged.Broadcast(bIsTalking);
}

bool UStarlightResponseSubsystem::GetStarlightTalking()
{
	return bIsStarlightTalking;
}
