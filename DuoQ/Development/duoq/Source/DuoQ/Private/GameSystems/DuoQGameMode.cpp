// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameSystems/DuoQGameMode.h"

#include "DuoQGameUserSettings.h"
#include "Characters/Partner/ChooseAreaGOAPAction.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Engine/PlayerStartPIE.h"
#include "UObject/ConstructorHelpers.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"
#include "Blueprint/UserWidget.h"
#include "Misc/OutputDeviceNull.h"
#include "GameSystems/Audio/AudioSubsystem.h"

ADuoQGameMode::ADuoQGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	// DefaultPawnClass = PlayerPawnClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> FadeWidgetClass(TEXT("/Game/UI/WBP_FadeToBlack"));
	if (FadeWidgetClass.Succeeded())
	{
		FadeWidget = FadeWidgetClass.Class;
	}

}

void ADuoQGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ReviveManager)
	{
		ReviveManager = NewObject<UReviveManager>(this, ReviveManagerClass);
	}
	AutoSelectScalability();
	int32 i = 0;
	FLevelBundle targetBundle = Levels[0];
	UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0));

	if (UAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
	{
		if (SaveGameInstance)
		{
			AudioSubsystem->SetMasterVolume(SaveGameInstance->MasterVolume);
			AudioSubsystem->SetMusicVolume(SaveGameInstance->MusicVolume);
			AudioSubsystem->SetSfxVolume(SaveGameInstance->SFXVolume);
			AudioSubsystem->SetVoiceVolume(SaveGameInstance->ChatVolume);
			AudioSubsystem->LoadCheckpointMusic(SaveGameInstance->PlayerProgress);
		}
		else
		{
			AudioSubsystem->SetMasterVolume(0.5f);
			AudioSubsystem->SetMusicVolume(0.5f);
			AudioSubsystem->SetSfxVolume(0.5f);
			AudioSubsystem->SetVoiceVolume(0.5f);
			AudioSubsystem->LoadCheckpointMusic(0);
		}
	}
	int count = 0;
	for (FLevelBundle bundle : Levels)
	{
		if (bundle.StoryProgress > count)
		{
			if (bundle.StoryProgress <= SaveGameInstance->PlayerProgress)
			{
				targetBundle = bundle;
				count = bundle.StoryProgress;
			}
		}
	}
	for (FName levels : targetBundle.Levels)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
		LatentInfo.UUID = i;
		i++;
		LatentInfo.Linkage = 0;
		UGameplayStatics::LoadStreamLevel(this, levels, true, true, LatentInfo);
	}
	NumLevelsLoading = targetBundle.Levels.Num();

	if (FadeWidget)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		FadeWidgetInstance = CreateWidget<UUserWidget>(PlayerController, FadeWidget);
		if (FadeWidgetInstance)
		{
			FadeWidgetInstance->AddToViewport(999);
			FOutputDeviceNull OutputDevice;
			FadeWidgetInstance->CallFunctionByNameWithArguments(TEXT("FadeOutAnimation"), OutputDevice, nullptr, true);
		}
	}
}
void ADuoQGameMode::OnLevelLoaded()
{
	NumLevelsLoaded++;
	bIsLevelLoaded = NumLevelsLoaded >= NumLevelsLoading;
	if (bIsLevelLoaded)
	{
		LevelLoaded.Broadcast();
	}
}

UReviveManager* ADuoQGameMode::GetReviveManager() const
{
	return ReviveManager;
}

AActor* ADuoQGameMode::ChoosePlayerStart_Implementation(AController* PlayerController)
{
	// Choose a player start
	APlayerStart* FoundPlayerStart = nullptr;
	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			FoundPlayerStart = PlayerStart;
			break;
		}
	}
	return FoundPlayerStart;
}

AActor* ADuoQGameMode::FindPlayerStart_Implementation(AController* PlayerController, const FString& IncomingName)
{
	return ChoosePlayerStart(PlayerController);
}

void ADuoQGameMode::AutoSelectScalability()
{
	// does basic benchmark test to apply scalability settings, but could be modified for specificity later
	if(UDuoQGameUserSettings* GameUserSettings = UDuoQGameUserSettings::GetDuoQGameUserSettings())
	{
		if (GameUserSettings->GetShouldRunHardwareBenchmark())
		{
			GameUserSettings->RunHardwareBenchmark();
			GameUserSettings->ApplyHardwareBenchmarkResults();
			GameUserSettings->SetShouldRunHardwareBenchmark(false);

			UE_LOG(LogTemp, Log, TEXT("Scalability Level: %d"), (GameUserSettings->GetOverallScalabilityLevel()));
		}
	}
}

void ADuoQGameMode::RegisterCombatManager(ACombatManager* CombatManager)
{
	ActiveCombatManagers.Add(CombatManager);
	OnCombatManagerRegistered.Broadcast(CombatManager);
}

void ADuoQGameMode::UnregisterCombatManager(ACombatManager* CombatManager)
{
	ActiveCombatManagers.Remove(CombatManager);
}

const TArray<ACombatManager*>& ADuoQGameMode::GetActiveCombatManager() const
{
	return ActiveCombatManagers;
}

