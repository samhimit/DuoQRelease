// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NNERuntimeCPU.h"
#include "SpeechRecognizer.h"
#include "Sound/CapturableSoundWave.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommunicationSubsystem.generated.h"

namespace UE::NNE {
	class IModelInstanceCPU;
}

class ULLMRequest;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeechRecognitionUpdate, const FString&, NewWords);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAPIRequestSent, const FString&, Request);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTranscriptionComplete, const FString&, SpeechBuffer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleMute, bool, bMuted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeechDetected, bool, bSpeechDetected);
DECLARE_LOG_CATEGORY_EXTERN(LogCommunication, Display, All);

class UCapturableSoundWave;
class USpeechRecognizer;

UENUM()
enum class ELLMProvider
{
	OpenAI,
	Gemini
};

/**
 * 
 */
UCLASS(Blueprintable,Abstract)
class DUOQ_API UCommunicationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

private:
	// Audio Input Device ID
	signed int SelectedAudioDeviceID = 0;
	friend class UChatGPTLLMRequest;
	friend class UMetricSubsystem;

	FDateTime RecognitionStart;
	FDateTime RecognitionComplete;
	FDateTime ResponseRecieved;

	FTimerHandle InputBufferTimer;

	bool bFailedLastDialogue = false;

	float FailurePercentage = 0.0f;

	/*TSharedPtr<UE::NNE::IModelInstanceCPU> ModelInstance;
	TArray<float> InputInputData;
	TArray<float> InputStateData;
	TArray<int64> InputSRData;
	TArray<float> OutputOutputData;
	TArray<float> OutputStateNData;
	TArray<UE::NNE::FTensorBindingCPU> InputBindings;
	TArray<UE::NNE::FTensorBindingCPU> OutputBindings;
	TArray<UE::NNE::FTensorShape> OutputTensorShapes;
	TArray<UE::NNE::FTensorShape> InputTensorShapes;

	void SetUpModelTensorBindings(const TArray<float>& PopulatedAudioData);*/

protected:

	UPROPERTY(EditAnywhere)
	float FailurePercentThreshold = 0.25f;
	
	UPROPERTY(EditDefaultsOnly)
	float AudioInputHangTime = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float LowQualityAudioInputHangTime = 0.4f;

	UPROPERTY(EditDefaultsOnly)
	int LowQualitySampleRateThreshold = 16000;

	int CurrentInputSampleRate = 0;
	
	int64 SampleRate = 8000;

	UPROPERTY(EditAnywhere)
	float RecognitionThreshold = 0.02;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNNEModelData> PreLoadedModelData;
	
	// Variables for LLM
	
	/** GOAP System prompt */
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Communication",  meta=(MultiLine="true"))
	FString GOAPSystemPrompt;
	/** Dialogue System prompt */
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Communication",  meta=(MultiLine="true"))
	FString DialogueSystemPrompt;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Communication",  meta=(MultiLine="true"))
	FString ConvoStartSystemPrompt;

	/** Max tokens for prompt */
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Communication",  meta=(MultiLine="true"))
	float MaxTokens = 4096.0f;

	/** What sampling temperature to use, between 0 and 2. Higher values like 0.8 will make the output more random, while lower values like 0.2 will make it more focused and deterministic.
		We generally recommend altering this or top_p but not both. */
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Communication",  meta=(MultiLine="true"))
	float Temperature = 1.0f;

	/** An alternative to sampling with temperature, called nucleus sampling, where the model considers the results of the tokens with top_p probability mass. So 0.1 means only the tokens comprising the top 10% probability mass are considered.
		We generally recommend altering this or temperature but not both. 0-1 scale */
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Communication",  meta=(MultiLine="true"))
	float Top_p = 1.0f;

	/** Step size for speech recognition */
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Communication",  meta=(MultiLine="true"))
	int32 SpeechStepSize = 3000;

	/** Model names*/
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Communication",  meta=(MultiLine="true"))
	FString ServerEndpoint = "";

	//GOAP Functions/variables
	class GOAPPlanner* Planner;

	// Audio input assets

	/** Speech recognizer to transcribe player input */
	UPROPERTY(BlueprintReadWrite, Category="Communication")
	TObjectPtr<USpeechRecognizer> SpeechRecognizer;

	/** Audio Input from player */
	UPROPERTY(BlueprintReadWrite, Category="Communication")
	TObjectPtr<UCapturableSoundWave> CapturableSoundWave;

	FOnSpeechRecognitionStartedDynamic OnSpeechRecognitionStartedDynamic;

	// Prompt history
	
	/** Current player's speech */
	UPROPERTY(BlueprintReadOnly, Category="Communication")
	FString InputPrompt;

	/** Array tracking goap conversations */
	UPROPERTY(BlueprintReadOnly, Category="Communication", meta=(MultiLine="true"))
	TArray<FString> GOAPConversationArray;

	/** Array tracking conversations */
	UPROPERTY(BlueprintReadOnly, Category="Communication", meta=(MultiLine="true"))
	TArray<FString> DialogueConversationArray;
	
	UPROPERTY()
	TObjectPtr<ULLMRequest> LLMRequest;

	// Communication functions

	/** Formats player input with visible objects from Starlight */
	UFUNCTION(BlueprintCallable)
	FString GenerateInputString(const FString& PlayerInput);

	/** Formats player input for a convo with Starlight */
	UFUNCTION(BlueprintCallable)
	FString GenerateDialogueInputString(const FString& PlayerInput);

	UFUNCTION(BlueprintCallable)
	FString GenerateConvoStartInputString(const FString& PlayerInput);

	/** Called on beginning of Speech Recognition */
	UFUNCTION()
	void OnStartSpeechRecognition(bool bSucceeded);
	
	/** Called when Recognition is stopped */
	UFUNCTION()
	void OnRecognitionEnd();

	UFUNCTION()
	void OnRecognitionError(const FString& ShortError,const FString& LongError);

	/** Called when the current processing progress updates */
	UFUNCTION()
	void OnRecognitionProgress(int32 Progress);

	/** Called when a phrase is recognized from the player */
	UFUNCTION()
	void OnRecognizedText(const FString& RecognizedWords);

	/** Called when the user has finished speaking */
	UFUNCTION()
	void SendBufferedWords();

	void CleanBufferedWords();

	/** Called when audio is available for transcribing */
	UFUNCTION()
	void OnPopulateAudioData(const TArray<float>& PopulatedAudioData);

	UFUNCTION()
	void APIRequestComplete(class UVaRestJsonObject* Request);
	
	UFUNCTION()
	void APIRequestFailed(class UVaRestRequestJSON* Request);
	// End of Communication functions

	UFUNCTION()
	void FinishAudioRecording();

public:
	UPROPERTY(EditDefaultsOnly)
	USoundBase* GOTOK;
	
	UPROPERTY(BlueprintAssignable)
	FOnSpeechRecognitionUpdate OnSpeechRecognitionUpdate;

	UPROPERTY(BlueprintAssignable)
	FOnToggleMute OnToggleMute;

	UPROPERTY(BlueprintAssignable)
	FOnSpeechDetected OnSpeechDetected;

	UPROPERTY(BlueprintAssignable)
	FOnAPIRequestSent OnAPIRequestSent;

	UPROPERTY(BlueprintAssignable)
	FOnTranscriptionComplete OnTranscriptionComplete;
	
	UFUNCTION(BlueprintCallable)
	void DebugStringInsert(const FString& Input);
	
	// goap
	/** Set the planner variable as the goapplanner of Starlight AI Controller*/
	void SetGOAPPlanner();

	/** set the planner variable as the goapplanner in the parameter */
	void SetGOAPPlanner(class GOAPPlanner* Target) { Planner = Target; }

	UFUNCTION()
	void PauseAudioCapture();
	
	UFUNCTION()
	void ResumeAudioCapture();

	UFUNCTION(BlueprintCallable)
	bool IsRecording();

	UFUNCTION(BlueprintCallable)
	void SetAudioDeviceID(signed int AudioDeviceID);

	UFUNCTION(BlueprintPure)
	signed int GetAudioDeviceID();

	UFUNCTION(BlueprintCallable)
	void PlaybackRecording();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TArray<FString> BackupCommands;

	mutable FCriticalSection BackupCommandsGuard;

	UFUNCTION(BlueprintCallable)
	void AddBackupInput(FString Input);

	UFUNCTION(BlueprintCallable)
	void RemoveBackupInput(FString Input);
	
	UPROPERTY(EditDefaultsOnly)
	bool bUseHumanReadableGoals = true;

	UPROPERTY(EditDefaultsOnly)
	bool bUseFlushing = true;

	UPROPERTY(EditDefaultsOnly)
	bool bClearWave = true;

	UPROPERTY(EditDefaultsOnly)
	bool bSendHistoryCallout = true;

	UPROPERTY(EditDefaultsOnly)
	bool bSendHistoryDialogue = true;

private:
	bool bIsFlusing = false;
	
	/** Recognized words that are buffered for sending */
	FString BufferedWords;

	/** Whether the words should be sent to the LLM after the next chunk of processed text */
	bool bAlreadySent = false;

	/** Whether a current audio segment is being processed by Whisper */
	bool bRecognitionInProgress = false;

	int32 RecognitionProgress = 0;

	FOnGetAvailableAudioInputDevicesResult DeviceResultDelegate;

	UFUNCTION()
	void UpdateInputDeviceSampleRate(const TArray<FRuntimeAudioInputDeviceInfo>& AvailableDevices);
};
