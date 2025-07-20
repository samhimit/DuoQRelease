#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "EnhancedInputSubsystems.h"
#include "GameSystems/SAMSubsystem/StarlightAffinityMeterSubsystem.h"
#include "DialogueSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueEnd,FString,BundleName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventRaised,FString,EventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventLowered,FString,EventName);
DECLARE_LOG_CATEGORY_EXTERN(LogDialogue, Display, All);


USTRUCT(BlueprintType)
struct FDollar : public FTableRowBase
{
	GENERATED_BODY()
	
	// The name of the Dollar, should be the same as the RowName in the Datatable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString Name;
	
	// The dialogue text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	FString Text; 

	// Indicates if this is the first line in its bundle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	bool IsInitial = true;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	bool bHasBeenSpent = false;	

	// IDs of the next possible Dollars
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	TArray<FString> NextDollars;

	// SAM change
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	int DollarSAMScore = 0;

	// Minimum SAM Level to unlock the dollar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	EStarlightAffectionLevel MinSAMLevel = EStarlightAffectionLevel::Abysmal;

	// Max SAM Level to unlock the dollar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	EStarlightAffectionLevel MaxSAMLevel = EStarlightAffectionLevel::Good;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<USoundBase> VoiceOver;

	// Cooldown for callouts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	float Cooldown = 0.0f;

	// The events that will get triggered after the dialogue is complete
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	TArray<FString> Events;

	// All the events that is needed for this dialogue to be triggered
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	TArray<FString> EventsNeeded;

	// The option that ACTIVATES this dialogue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	FString OptionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	FString HelperText; 
};

USTRUCT(Blueprintable, BlueprintType)
struct FCalloutParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDataTable* CalloutTable;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldOverrideCallout = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Cooldown = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ChanceToTrigger = 1.0f;
};

UENUM(BlueprintType)
enum class EBundleQueueMode:uint8
{
	WaitForCurrentVoiceLine, WaitForCurrentBundle
};

UCLASS(Blueprintable,Abstract)
class DUOQ_API UDialogueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Function to activate a bundle
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool ActivateBundle(const FString& BundleName);

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetAllEventsTriggered() { return AllEventsTriggered; } 

    // Function to display the next dialogue (Dollar)
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void ActivateNextDollar();

	// Function to activate the dollar passed as parameter
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool ActivateDollar(const FString& DollarName);

    // Resets all dialogues to their default state
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void ResetDialogue();

    // Closes the current dialogue
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void CloseDialogue(const FString& TableID);

	// Start a callout conversation
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool DoCallout(FString CalloutTableName);

	// Start refocusing lines
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void PlayRefocusingLines();

	// Generates the formatted text for one dollar
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FString GenerateDollarText(const FString& DollarName);

	// Generates the formatted text for all the next dollars that have not been spent
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FString GenerateNextDollarsText(const FString& DollarName);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<FString> GetNextDollars(const FString DollarName);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<FString> GetNextDollarsWithPrecondition(const FString DollarName);

	// Should return true if is in conversation or callout or refocusing line
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool GetIsInConversation() const {return bIsInConversation || bIsInCallout || bIsRefocusing;}

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool GetIsInNarrativeConversation() const {return bIsInConversation;}

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool GetIsStarlightTalking() const { return bIsStarlightTalking; }

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool CheckDollarPrecondition(const FString& DollarName);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AddCompletedEvent(FString Event);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void OnEventTriggered(FString EventName);

    FString GetCurrentDollarName();

	FDollar* GetCurrentDollar();

	// Returns the next options, only return the ones that have their dialogue unlocked
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<FString> GetNextOptions(const FString& DollarName);

	// Display the options on the HUD
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void DisplayOptions();

	// Hides the options on the HUD, works for main menu and in game
	UFUNCTION(Blueprintable, Category = "Dialogue")
	void HideOptions() const;

	UFUNCTION(Blueprintable, Category = "Dialogue")
	void HideDialogue() const;

	// Highlights the chosen option until the next options gets displayed
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void HighlightOption(FString DollarName);

	// Called when Starlight starts talking
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void HandleStarlightTalkingChanged(bool bIsTalking);

	UFUNCTION(Category = "Dialogue")
	void SetLastUserMessage(FString UserMessage);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ClearTrigger();

	// Datatables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TMap<FString, UDataTable*> DollarTables;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TMap<FString, FCalloutParameters> CalloutTables;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	UDataTable* RefocusingLines;

	// UI Classes and instances
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	UUserWidget* DialogueWidgetInstance;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> DialogueWidgetClass;

	// Input action for dialogue progression
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	UInputAction* InputAction_NextDialogue;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(BlueprintAssignable)
	FOnDialogueEnd OnDialogueEnd;

	UPROPERTY(BlueprintAssignable)
	FOnEventRaised OnEventRaised;

	UPROPERTY(BlueprintAssignable)
	FOnEventLowered OnEventLowered;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	EBundleQueueMode QueueMode = EBundleQueueMode::WaitForCurrentVoiceLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TMap<FString, FString> ConvoStarts;

	UFUNCTION(BlueprintCallable)
	bool CheckEventTriggered(FString Event);

	UFUNCTION(BlueprintCallable)
	void RemoveEvent(FString Event);

	bool bBlockDialogue = false;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	FString CurrentDollarName;
	
	FDollar* CurrentDollar;
	
	UPROPERTY(BlueprintReadOnly)
	FString CurrentBundleName;

	UPROPERTY(BlueprintReadOnly)
	FString BundleInQueue;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FString IgnoreHelperTextString = "Unpickable";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	float QueuedBundleDelay = 1.0f;

private:
	FDollar* GetDollar(const FString& DollarName);
	
	bool bIsInConversation; // Boolean to detect whether is in narrative convos
	bool bIsInCallout; // Boolean to detect whether is in callout convos
	bool bIsRefocusing; // Boolean to detect whether is in refocusing convos
	bool bIsStarlightTalking; // Boolean to detect if Starlight talking (for subtitles)
	
	// String to store the last user request for metrics purposes
	FString LastUserMessage;

	// Initializes player controls related to Dialogue, uses a timer to ensure player is ready before initializing
	FTimerHandle InitTimerHandle;
	void InitializePlayerControls();

	// Stores all the dialogue events that have been triggered
	TArray<FString> AllEventsTriggered;

	// Stores the cooldowns of the callouts
	TMap<FString, float> ActiveCalloutCooldowns;
	TMap<FString, FTimerHandle> CalloutCooldownTimers;

	FTimerHandle OptionTimerHandle;
	FTimerHandle CompletionTimerHandle;

	// Timer for queued dialogues
	FTimerHandle BundleQueueTimerHandle;

	UFUNCTION()
	void ResetCalloutCooldown(const FString& CalloutTableName);

	UFUNCTION()
	void DelayedHideAndDisplayOptions();

	UFUNCTION()
	void CheckDialogueCompletion();

	UFUNCTION()
	void DelayedActivateBundleInQueue();
};
