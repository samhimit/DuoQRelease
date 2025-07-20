#include "GameSystems/Dialogue/DialogueSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainMenuGameMode.h"
#include "MovieSceneSequencePlayer.h"
#include "GameSystems/SAMSubsystem/StarlightAffinityMeterSubsystem.h"
#include "GameSystems/Dialogue/StarlightResponseSubsystem.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "UI/DialogueWidget.h"
#include "UI/IntroSequenceHUD.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"
#include "GameSystems/Metrics/MetricSubsystem.h"
#include "GameSystems/Progression/CreditsSubsystem.h"
#include "IntroSequence/MainMenuPawn.h"
#include "UI/GameplayHUD.h"
#include "UI/OffscreenIndicatorWidget.h"

DEFINE_LOG_CATEGORY(LogDialogue);

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Set up player input bindings for dialogue
	// Start the timer to check for the player character's readiness
	// GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &UDialogueSubsystem::InitializePlayerControls, 0.1f, true);

	// Call reset to make sure every dialogue is reset to unspent.
	ResetDialogue();
	if (UStarlightResponseSubsystem* SRS = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
	{
		SRS->OnStarlightTalkingChanged.AddDynamic(this, &UDialogueSubsystem::HandleStarlightTalkingChanged);
	}

	OnEventRaised.AddDynamic(this, &UDialogueSubsystem::OnEventTriggered);
}

void UDialogueSubsystem::Deinitialize()
{
	Super::Deinitialize();
	ResetDialogue();
}

// Delayed initialization
void UDialogueSubsystem::InitializePlayerControls()
{

	if (ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			// Check if InputMappingContext and InputAction_NextDialogue are valid
			if (InputMappingContext == nullptr || InputAction_NextDialogue == nullptr)
			{
				UE_LOG(LogDialogue, Warning, TEXT("InputMappingContext or InputAction_NextDialogue is nullptr!"));
				return;
			}

			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				// Add the input mapping context
				InputSubsystem->AddMappingContext(InputMappingContext, 1);
			}
			else
			{
				UE_LOG(LogDialogue, Warning, TEXT("InputSubsystem is nullptr!"));
				return;
			}

			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				// Bind the input action
				EnhancedInputComponent->BindAction(InputAction_NextDialogue, ETriggerEvent::Triggered, this, &UDialogueSubsystem::ActivateNextDollar);
				UE_LOG(LogDialogue, Log, TEXT("PlayerCharacter found, input bound for dialogue subsystem."));
			}
			else
			{
				UE_LOG(LogDialogue, Warning, TEXT("Failed to cast PlayerController's InputComponent to EnhancedInputComponent."));
			}

			// Stop checking once the initialization is done
			GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
		}
		else
		{
			UE_LOG(LogDialogue, Warning, TEXT("PlayerController not found."));
		}
	}
	else
	{
		UE_LOG(LogDialogue, Warning, TEXT("PlayerCharacter not found."));
	}
}

bool UDialogueSubsystem::ActivateBundle(const FString& BundleName)
{
	// Override all current conversations and start this new bundle.
	// CloseDialogue(CurrentBundleName);
	bool bStartedNewConversation = false;

	// prevent doubling up on an already playing dialogue
	if (CurrentBundleName == BundleName)
	{
		return bStartedNewConversation;
	}

	// Save previous state so that if the bundle activation failed it could be reverted
	FString PrevBundleName = CurrentBundleName;
	bool	bWasInConversation = bIsInConversation;

	// Making sure that all helper/checker functions work within the scope of the new bundle
	bIsInConversation = true;
	CurrentBundleName = BundleName;

	if (DollarTables.Contains(BundleName))
	{
		if (const UDataTable* DollarTable = DollarTables[BundleName])
		{
			TArray<FDollar*> Dollars;
			DollarTable->GetAllRows<FDollar>(TEXT(""), Dollars);
			// If the bundle is empty don't activate it
			if (Dollars.IsEmpty() || bBlockDialogue)
			{
				UE_LOG(LogDialogue, Warning, TEXT("Empty Bundle being activated!"));
				// Revert to previous state because new bundle activation failed
				CurrentBundleName = PrevBundleName;
				bIsInConversation = bWasInConversation;
				return bStartedNewConversation;
			}

			// Check if the bundle has any valid dollars in it
			bool bHasValidDollar = false;
			for (const auto Dollar : Dollars)
			{
				if (Dollar->IsInitial && CheckDollarPrecondition(Dollar->Name))
				{
					bHasValidDollar = true;
				}
			}
			// If there isn't any valid dollar in the bundle, return
			if (!bHasValidDollar)
			{
				// Revert to previous bundle name because new bundle activation failed
				CurrentBundleName = PrevBundleName;
				bIsInConversation = bWasInConversation;
				return bStartedNewConversation;
			}

			// Check if Starlight is talking, if she's talking add the Bundle to the queue

			if (bIsStarlightTalking)
			{
				BundleInQueue = BundleName;
				CurrentBundleName = PrevBundleName;
				bIsInConversation = bWasInConversation;
				return false;
			}
			FString StartingDollar = TEXT_EMPTY;
			// UE_LOG(LogDialogue, Warning, TEXT("Current bundle: %s"),*CurrentBundleName);
			for (const auto Dollar : Dollars)
			{
				// Make all dollars unspent
				Dollar->bHasBeenSpent = false;
				if (Dollar->IsInitial && CheckDollarPrecondition(Dollar->Name))
				{
					StartingDollar = Dollar->Name;
					// break; probably shouldn't break out of the loop anymore
				}
			}
			if (!StartingDollar.IsEmpty())
			{
				bStartedNewConversation = ActivateDollar(StartingDollar);
				HideOptions();
				bIsInConversation = bStartedNewConversation;
			}
		}
	}

	return bStartedNewConversation;
}

void UDialogueSubsystem::ActivateNextDollar()
{
	HideOptions();

	if (FDollar* GrabbedCurrentDollar = GetDollar(CurrentDollarName))
	{
		for (const FString& NextDollarName : GrabbedCurrentDollar->NextDollars)
		{
			FDollar* NextDollar = GetDollar(NextDollarName);
			if (NextDollar && !NextDollar->bHasBeenSpent)
			{
				// Logging the formatted next dollars text
				FString DollarInfo = GenerateNextDollarsText(NextDollarName);
				UE_LOG(LogDialogue, Log, TEXT("%s"), *DollarInfo);

				if (UStarlightResponseSubsystem* StarlightResponseSubsystem = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
				{
					StarlightResponseSubsystem->HandleDialogue(NextDollar->Name, NextDollar->Text, NextDollar->VoiceOver);
				}
				CurrentDollarName = NextDollarName;
				NextDollar->bHasBeenSpent = true;

				break; // Activate only one next dialogue
			}
		}
	}
	else
	{
		UE_LOG(LogDialogue, Warning, TEXT("Failed to get next dollar"));
	}
}

bool UDialogueSubsystem::ActivateDollar(const FString& DollarName)
{
	// Handle special responses for "End" and "Bad"
	if (DollarName == "End")
	{
		if (UMetricSubsystem* MetricSubsystem = GetGameInstance()->GetSubsystem<UMetricSubsystem>())
		{
			MetricSubsystem->TrackFailedDollar(CurrentDollarName, LastUserMessage);
		}

		if (CurrentDollarName.IsEmpty())
		{
			CloseDialogue(CurrentBundleName);
		}
		else
		{
			// Replay the current dollar
			FDollar* Dollar = GetDollar(CurrentDollarName);
			if (Dollar != nullptr)
			{
				if (UStarlightResponseSubsystem* StarlightResponseSubsystem = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
				{
					TArray<FDollar*> Dollars;
					RefocusingLines->GetAllRows<FDollar>(TEXT("Refocusing"), Dollars);
					int32 RandomIndex = FMath::RandRange(0, Dollars.Num() - 1);
					StarlightResponseSubsystem->HandleDialogue(Dollar->Name, Dollar->Text, Dollars[RandomIndex]->VoiceOver);
				}
			}
		}
		return false;
	}

	if (DollarName == "Bad")
	{
		if (UMetricSubsystem* MetricSubsystem = GetGameInstance()->GetSubsystem<UMetricSubsystem>())
		{
			MetricSubsystem->TrackFailedDollar(CurrentDollarName, LastUserMessage);
		}

		if (UStarlightAffinityMeterSubsystem* SAMSubsystem = GetGameInstance()->GetSubsystem<UStarlightAffinityMeterSubsystem>())
		{
			SAMSubsystem->UpdateSAMScore(-1);
		}
		CloseDialogue(CurrentBundleName);
		return false;
	}

	// Normal dollar handling

	FDollar* Dollar = GetDollar(DollarName);

	if (Dollar && CheckDollarPrecondition(DollarName))
	{
		if (Dollar->bHasBeenSpent != true)
		{
			// Update SAMScore
			if (UStarlightAffinityMeterSubsystem* SAMSubsystem = GetGameInstance()->GetSubsystem<UStarlightAffinityMeterSubsystem>())
			{
				if (Dollar->DollarSAMScore != 0)
				{
					SAMSubsystem->UpdateSAMScore(Dollar->DollarSAMScore);
					UE_LOG(LogDialogue, Log, TEXT("Updated SAM Score"));
				}
			}
			CurrentDollarName = DollarName;
			CurrentDollar = Dollar;

			// If there is a voiceover, calculate the delay for hiding and displaying options
			float DelayTime = 2.0f; // Default to 2 seconds if no voiceover is found
			if (Dollar->VoiceOver)
			{
				DelayTime = Dollar->VoiceOver->GetDuration() / 4.0f; // Delay for 1/4 of the voiceover length
			}
			// Set the timer for HideOptions and DisplayOptions
			GetWorld()->GetTimerManager().SetTimer(OptionTimerHandle, this, &UDialogueSubsystem::DelayedHideAndDisplayOptions, DelayTime, false);

			// Handle displaying dollar subtitle and playing voice over
			if (UStarlightResponseSubsystem* StarlightResponseSubsystem = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
			{
				StarlightResponseSubsystem->HandleDialogue(Dollar->Name, Dollar->Text, Dollar->VoiceOver);
			}

			HighlightOption(DollarName);
			Dollar->bHasBeenSpent = true;

			// Broadcast events bound to this dollar
			for (FString Event : Dollar->Events)
			{
				OnEventRaised.Broadcast(Event);
			}

			// Set a delayed check for dialogue completion after the vo ends
			float CompletionDelay = DelayTime * 4 + 0.1f; // Adding a slight extra delay to ensure the options are shown before the dialogue closes
			GetWorld()->GetTimerManager().SetTimer(CompletionTimerHandle, this, &UDialogueSubsystem::CheckDialogueCompletion, CompletionDelay, false);
			return true;
		}
		else
		{
			UE_LOG(LogDialogue, Warning, TEXT("Tried to activate an already activated dollar: %s"), *DollarName);
		}
	}
	else
	{
		if (CurrentDollar && !CurrentDollar->NextDollars.IsEmpty())
		{
			ActivateDollar("End");
		}
		else
		{
			CloseDialogue(CurrentBundleName);
		}
		UE_LOG(LogDialogue, Warning, TEXT("Dollar does not exist: %s"), *DollarName);
	}
	return false;
}

void UDialogueSubsystem::CloseDialogue(const FString& TableID)
{
	HideOptions();
	HideDialogue();

	bool bWasInConversation = bIsInConversation;
	OnDialogueEnd.Broadcast(TableID);
	bIsInConversation = false;
	bIsInCallout = false;
	bIsRefocusing = false;
	CurrentBundleName = "";
	CurrentDollarName = "";

	// If the dialogue being closed is a narrative convo:
	// if the queue mode is set to WaitForCurrentBundle, activate the bundle in queue
	if (bWasInConversation && !BundleInQueue.IsEmpty() && QueueMode == EBundleQueueMode::WaitForCurrentBundle)
	{
		GetWorld()->GetTimerManager().SetTimer(BundleQueueTimerHandle, this, &UDialogueSubsystem::DelayedActivateBundleInQueue, QueuedBundleDelay);
	}
}

bool UDialogueSubsystem::DoCallout(FString CalloutTableName)
{
	const FCalloutParameters* Callout = CalloutTables.Find(CalloutTableName);

	if (!Callout)
	{
		return false;
	}

	// Should never override narrative convos
	if (bIsInConversation || bBlockDialogue)
	{
		return false;
	}

	// Should only override callout convos if bShouldOverrideCurrentCallout is true
	if (bIsInCallout && !Callout->bShouldOverrideCallout)
	{
		return false;
	}

	// Check if the callout is on cooldown
	if (ActiveCalloutCooldowns.Contains(CalloutTableName) && ActiveCalloutCooldowns[CalloutTableName] > 0.0f)
	{
		// UE_LOG(LogDialogue, Warning, TEXT("Callout not triggered due to cooldown"));
		return false;
	}

	// Handle the chance to trigger
	if (FMath::FRand() > Callout->ChanceToTrigger)
	{
		// UE_LOG(LogDialogue, Warning, TEXT("Callout not triggered due to chance"));
		return false;
	}

	TArray<FDollar*> Dollars;
	Callout->CalloutTable->GetAllRows<FDollar>(TEXT(""), Dollars);
	TArray<FDollar*> InitialDollars;
	CurrentBundleName = CalloutTableName;
	bIsInCallout = true;

	// Collect all initial dollars that meet the condition
	for (auto Dollar : Dollars)
	{
		if (Dollar->IsInitial && CheckDollarPrecondition(Dollar->Name) && !Dollar->bHasBeenSpent)
		{
			InitialDollars.Add(Dollar);
		}
	}

	// Randomly pick one of the initial dollars
	if (InitialDollars.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, InitialDollars.Num() - 1);
		if (ActivateDollar(InitialDollars[RandomIndex]->Name))
		{
			// Start the cooldown for this callout if applicable
			if (Callout->Cooldown > 0.0f)
			{
				ActiveCalloutCooldowns.Add(CalloutTableName, Callout->Cooldown);

				// Create a timer delegate for the cooldown
				FTimerDelegate CalloutTimerDelegate = FTimerDelegate::CreateUFunction(this, FName("ResetCalloutCooldown"), CalloutTableName);

				// Create a timer handle for the cooldown and set the timer
				FTimerHandle CalloutTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(CalloutTimerHandle, CalloutTimerDelegate, Callout->Cooldown, false);

				// Store the timer handle
				CalloutCooldownTimers.Add(CalloutTableName, CalloutTimerHandle);
			}
			else
			{
				InitialDollars[RandomIndex]->bHasBeenSpent = false;
			}
			return true;
		}
	}
	else
	{
		CloseDialogue(CurrentBundleName);
	}

	return false;
}

void UDialogueSubsystem::PlayRefocusingLines()
{
	bIsInCallout = false;
	bIsRefocusing = true;

	TArray<FDollar*> Dollars;
	RefocusingLines->GetAllRows<FDollar>(TEXT("Refocusing"), Dollars);
	int32 RandomIndex = FMath::RandRange(0, Dollars.Num() - 1);
	ActivateDollar(Dollars[RandomIndex]->Name);
}

FString UDialogueSubsystem::GenerateDollarText(const FString& DollarName)
{
	FDollar* Dollar = GetDollar(DollarName);

	if (!Dollar)
	{
		UE_LOG(LogDialogue, Warning, TEXT("Dollar not found when generating dollar text!"));
		return FString("Invalid Dollar");
	}

	FString DollarText = Dollar->Text;

	if (!Dollar->HelperText.IsEmpty())
	{
		DollarText += TEXT(" (") + Dollar->HelperText + TEXT(")");
	}

	FString result = FString::Printf(TEXT("\"%s\":\"%s\",\n"), *Dollar->Name, *DollarText);
	return result;
}

FString UDialogueSubsystem::GenerateNextDollarsText(const FString& DollarName)
{
	FString	 Result = "";
	FDollar* Dollar = GetDollar(DollarName);
	if (!Dollar)
	{
		UE_LOG(LogDialogue, Warning, TEXT("Dollar not found when generating next dollar text!"));
		return FString("Invalid Dollar");
	}

	for (const FString& NextDollarName : Dollar->NextDollars)
	{
		if (FDollar* NextDollar = GetDollar(NextDollarName); NextDollar && NextDollar->HelperText.Compare(IgnoreHelperTextString, ESearchCase::IgnoreCase) != 0)
		{
			// only consider dollars that: have not been spent, have SAM preconditions met, and have event preconditions met
			if (!NextDollar->bHasBeenSpent && CheckDollarPrecondition(NextDollarName))
			{
				Result += GenerateDollarText(NextDollarName);
			}
		}
	}

	return Result;
}

bool UDialogueSubsystem::CheckDollarPrecondition(const FString& DollarName)
{
	if (FDollar* Dollar = GetDollar(DollarName))
	{
		if (!Dollar->EventsNeeded.IsEmpty())
		{
			for (FString Event : Dollar->EventsNeeded)
			{
				// Return false if any of the precondition is not satisfied
				if (AllEventsTriggered.Find(Event) == INDEX_NONE)
				{
					return false;
				}
			}
		}
		if (UStarlightAffinityMeterSubsystem* SAMSubsystem = GetGameInstance()->GetSubsystem<UStarlightAffinityMeterSubsystem>())
		{
			if (!(SAMSubsystem->CurrentSAMLevel >= Dollar->MinSAMLevel && SAMSubsystem->CurrentSAMLevel <= Dollar->MaxSAMLevel))
			{
				return false;
			}
		}

		// Return true if EventsNeeded is empty or if all Events in EventsNeeded are satisfied && SAM condition satisfied
		return true;
	}

	UE_LOG(LogDialogue, Warning, TEXT("Cannot find Dollar %s when checking precondition"), *DollarName);
	return false;
}

TArray<FString> UDialogueSubsystem::GetNextOptions(const FString& DollarName)
{
	TArray<FString> Options;
	TArray<FString> NextDollars = GetNextDollarsWithPrecondition(DollarName);

	for (auto NextDollarName : NextDollars)
	{
		if (FDollar* Dollar = GetDollar(NextDollarName))
		{
			if (!Dollar->OptionText.IsEmpty() && !Dollar->bHasBeenSpent)
			{
				Options.Add(Dollar->OptionText);
			}
		}
	}

	return Options;
}

void UDialogueSubsystem::DisplayOptions()
{
	TArray<FString> DollarNames = GetNextDollarsWithPrecondition(CurrentDollarName);
	TArray<FDollar> NextDollars;

	int RevealedOptionCount = 0;
	for (auto DollarName : DollarNames)
	{
		if (FDollar* Dollar = GetDollar(DollarName); Dollar && !Dollar->bHasBeenSpent)
		{
			NextDollars.Add(*Dollar);
			if (FMath::IsNearlyZero(Dollar->Cooldown))
			{
				RevealedOptionCount++;
			}
		}
	}

	if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
	{
		DuoQPlayerCharacter->PlayerHUD->DialogueHUD->DelayShowOptions(NextDollars, RevealedOptionCount == 1);
	}
	else if (AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (MainMenuGameMode->MenuHUD)
		{
			if (MainMenuGameMode->MenuHUD->DialogueHUD)
			{
				MainMenuGameMode->MenuHUD->DialogueHUD->DelayShowOptions(NextDollars, RevealedOptionCount == 1);
			}
		}
	}
}

void UDialogueSubsystem::HideOptions() const
{
	if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
	{
		if (UDuoQHUD* HUD = DuoQPlayerCharacter->PlayerHUD)
		{
			if (UDialogueWidget* DW = HUD->DialogueHUD)
			{
				DW->HideOptions();
			}
		}
	}
	else if (AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (UIntroSequenceHUD* IS = MainMenuGameMode->MenuHUD)
		{
			if (UDialogueWidget* DW = IS->DialogueHUD)
			{
				DW->HideOptions();
			}
		}
	}
}

void UDialogueSubsystem::HideDialogue() const
{
	if (!bIsStarlightTalking)
	{
		if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
		{
			DuoQPlayerCharacter->PlayerHUD->DialogueHUD->HideSubtitle();
		}
		else if (AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			if (MainMenuGameMode->MenuHUD)
			{
				if (MainMenuGameMode->MenuHUD->DialogueHUD)
				{
					MainMenuGameMode->MenuHUD->DialogueHUD->HideSubtitle();
				}
			}
		}
	}
}

void UDialogueSubsystem::HighlightOption(FString DollarName)
{
	if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
	{
		if (DuoQPlayerCharacter->PlayerHUD != nullptr)
		{
			if (DuoQPlayerCharacter->PlayerHUD->DialogueHUD != nullptr)
			{
				DuoQPlayerCharacter->PlayerHUD->DialogueHUD->HighlightOption(DollarName);
			}
		}
	}
	else if (AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (MainMenuGameMode->MenuHUD != nullptr)
		{
			if (MainMenuGameMode->MenuHUD->DialogueHUD != nullptr)
			{
				MainMenuGameMode->MenuHUD->DialogueHUD->HighlightOption(DollarName);
			}
		}
	}
	UE_LOG(LogDialogue, Warning, TEXT("Option Highlighted: %s"), *DollarName);
}

void UDialogueSubsystem::HandleStarlightTalkingChanged(bool bIsTalking)
{
	if (bIsTalking)
	{
		UE_LOG(LogDialogue, Warning, TEXT("Starlight starts talking"));
		bIsStarlightTalking = true;
	}
	else
	{
		UE_LOG(LogDialogue, Warning, TEXT("Starlight finishes talking"));
		bIsStarlightTalking = false;
		if (!bIsInConversation)
		{
			HideDialogue();
		}

		if (!BundleInQueue.IsEmpty() && QueueMode == EBundleQueueMode::WaitForCurrentVoiceLine)
		{
			GetWorld()->GetTimerManager().SetTimer(BundleQueueTimerHandle, this, &UDialogueSubsystem::DelayedActivateBundleInQueue, QueuedBundleDelay);
		}
	}
}

void UDialogueSubsystem::ResetDialogue()
{
	// Reset status of all dollars in DollarTable
	for (const auto& DollarTablePair : DollarTables)
	{
		const UDataTable* DollarTable = DollarTablePair.Value;
		if (DollarTable != nullptr)
		{
			TArray<FName> DollarNames = DollarTable->GetRowNames();
			for (const FName& DollarName : DollarNames)
			{
				if (FDollar* Dollar = DollarTable->FindRow<FDollar>(DollarName, TEXT("ResetDollar")))
				{
					Dollar->bHasBeenSpent = false;
				}
			}
		}
		else
		{
			UE_LOG(LogDialogue, Error, TEXT("Dollartable set to null"));
		}
	}

	for (const auto& CalloutParametersPair : CalloutTables)
	{
		const FCalloutParameters CalloutParameters = CalloutParametersPair.Value;
		const UDataTable*		 CalloutTable = CalloutParameters.CalloutTable;
		if (CalloutTable != nullptr)
		{
			TArray<FName> DollarNames = CalloutTable->GetRowNames();
			for (const FName& DollarName : DollarNames)
			{
				if (FDollar* Dollar = CalloutTable->FindRow<FDollar>(DollarName, TEXT("ResetDollar")))
				{
					Dollar->bHasBeenSpent = false;
				}
			}
		}
		else
		{
			UE_LOG(LogDialogue, Error, TEXT("Callouttable set to null"));
		}
	}

	CurrentDollarName = "";
	CurrentBundleName = "";
	bIsInConversation = false;
	bIsInCallout = false;
	bIsRefocusing = false;
	UE_LOG(LogDialogue, Log, TEXT("All dialogues have been reset to Unspent state."));
}

void UDialogueSubsystem::AddCompletedEvent(FString Event)
{
	AllEventsTriggered.Add(Event);
}

void UDialogueSubsystem::OnEventTriggered(FString EventName)
{
	AddCompletedEvent(EventName);

	if (EventName == "EndGame")
	{
		bBlockDialogue = true;
		if (UCreditsSubsystem* CreditsSubsystem = GetGameInstance()->GetSubsystem<UCreditsSubsystem>())
		{
			OnDialogueEnd.AddDynamic(CreditsSubsystem, &UCreditsSubsystem::StartCreditsSequence);
		}
	}
	if (EventName == "MenuEnd")
	{
		bBlockDialogue = true;
		if (UCreditsSubsystem* CreditsSubsystem = GetGameInstance()->GetSubsystem<UCreditsSubsystem>())
		{
			OnDialogueEnd.AddDynamic(CreditsSubsystem, &UCreditsSubsystem::StartCreditsFromMenu);
		}
	}
	if (EventName == "RevertStarlight")
	{
		if (UStarlightResponseSubsystem* ResponseSubsystem = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
		{
			ResponseSubsystem->bIsStarlight = true;
		}
		if(UDuoQHUD* DuoQHUD = UDuoQBlueprintFunctionLibrary::GetHUDInstance(this))
		{
			DuoQHUD->DialogueHUD->RevertStarlight();
			DuoQHUD->RevertStarlight();
			if(ADuoQPlayerController* Controller = UDuoQBlueprintFunctionLibrary::GetDuoQPlayerController(this,0))
			{
				Controller->GetOffscreenIndicatorWidget()->bShouldBeHiddenPermanently = true;
			}
		}
	}
}

void UDialogueSubsystem::ResetCalloutCooldown(const FString& CalloutTableName)
{
	// Remove the cooldown from the active cooldown list
	ActiveCalloutCooldowns.Remove(CalloutTableName);
	CalloutCooldownTimers.Remove(CalloutTableName);
}

void UDialogueSubsystem::DelayedHideAndDisplayOptions()
{
	HideOptions();
	DisplayOptions();
}

void UDialogueSubsystem::CheckDialogueCompletion()
{
	bool bDialogueComplete = true;
	if (FDollar* Dollar = GetDollar(CurrentDollarName))
	{
		for (auto DollarString : Dollar->NextDollars)
		{
			if (!DollarString.IsEmpty())
			{
				FDollar* NextDollar = GetDollar(DollarString);
				if (NextDollar && !NextDollar->bHasBeenSpent)
				{
					bDialogueComplete = false;
					break;
				}
			}
		}
	}

	if (bDialogueComplete)
	{
		CloseDialogue(CurrentBundleName);
	}
}

void UDialogueSubsystem::DelayedActivateBundleInQueue()
{
	ActivateBundle(BundleInQueue);
	BundleInQueue = "";
}

FString UDialogueSubsystem::GetCurrentDollarName()
{
	return CurrentDollarName;
}

FDollar* UDialogueSubsystem::GetCurrentDollar()
{
	return CurrentDollar;
}

bool UDialogueSubsystem::CheckEventTriggered(FString Event)
{
	return AllEventsTriggered.Contains(Event);
}

void UDialogueSubsystem::RemoveEvent(FString Event)
{
	if (AllEventsTriggered.Remove(Event) > 0)
	{
		OnEventLowered.Broadcast(Event);
	}
}

FDollar* UDialogueSubsystem::GetDollar(const FString& DollarName)
{
	if (!DollarName.IsEmpty())
	{
		if (bIsInConversation)
		{
			if (DollarTables.Contains(CurrentBundleName))
			{
				if (FDollar* Dollar = DollarTables[CurrentBundleName]->FindRow<FDollar>(FName(*DollarName), ""))
				{
					return Dollar;
				}
			}
		}
		else if (bIsInCallout)
		{
			if (CalloutTables.Contains(CurrentBundleName))
			{
				if (FDollar* Dollar = CalloutTables[CurrentBundleName].CalloutTable->FindRow<FDollar>(FName(*DollarName), ""))
				{
					return Dollar;
				}
			}
		}
		else if (bIsRefocusing)
		{
			if (FDollar* Dollar = RefocusingLines->FindRow<FDollar>(FName(*DollarName), ""))
			{
				return Dollar;
			}
		}
	}
	UE_LOG(LogDialogue, Warning, TEXT("Incorrect DollarName in GetDollar"));
	// CloseDialogue(CurrentBundleName);
	return nullptr;
}

TArray<FString> UDialogueSubsystem::GetNextDollars(const FString DollarName)
{
	if (FDollar* Dollar = GetDollar(DollarName))
	{
		return Dollar->NextDollars;
	}
	return TArray<FString>();
}

TArray<FString> UDialogueSubsystem::GetNextDollarsWithPrecondition(const FString DollarName)
{
	TArray<FString> Result;
	TArray<FString> NextDollars = GetNextDollars(DollarName);
	for (auto NextDollarName : NextDollars)
	{
		if (FDollar* Dollar = GetDollar(NextDollarName))
		{
			if (CheckDollarPrecondition(NextDollarName))
			{
				Result.Add(NextDollarName);
			}
		}
	}
	return Result;
}

void UDialogueSubsystem::SetLastUserMessage(FString UserMessage)
{
	LastUserMessage = UserMessage;
}

void UDialogueSubsystem::ClearTrigger()
{
	AllEventsTriggered.Empty();
}