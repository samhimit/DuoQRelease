// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionStart, FString, InputName);

/**
 * Auxiliary struct to store info about music for each checkpoint
 */
USTRUCT(BlueprintType, Blueprintable)
struct FCheckpointMusic
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CheckpointMusic)
	int CheckpointNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CheckpointMusic)
	TSubclassOf<class UMusicManager> MusicManagerClass;
};

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class DUOQ_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Music")
	TObjectPtr<class UMusicManager> ActiveMusicPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "Music")
	TObjectPtr<class UMusicManager> ReadyMusicPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "Music")
	TObjectPtr<class UMusicManager> EndingMusicPlayer;

	UPROPERTY(EditDefaultsOnly, Category = "Music")
	TArray<FCheckpointMusic> CheckpointMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TMap<FString, TSubclassOf<class UMusicManager>> EventMusicMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	FString ComplimentStartEventName = "ComplimentStart";

	UPROPERTY(BlueprintReadWrite, Category = "SFX")
	TObjectPtr<class UAudioComponent> PersistentSFXPlayer;

	// Mixing

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundClass> MasterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundMix> MasterMix;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundClass> MusicClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundMix> MusicMix;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundClass> SfxClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundMix> SfxMix;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundClass> VoiceClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundMix> VoiceMix;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundClass> InputDuckingClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	TObjectPtr<USoundMix> InputDuckingMix;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	class USoundControlBusMix* ConversationMix;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	class USoundControlBusMix* CombatMix;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	class USoundControlBusMix* PauseMix;
	
	// How much audio ducks when reading voice input
	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	float InputDuckingModifer = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Mixing")
	float InputDuckingFadeTime = 0.7f;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Music")
	bool bAddLayerOnNextEnemy = false;

	UPROPERTY(BlueprintReadWrite, Category = "Music")
	bool bBlockNewMusic = false;

	UPROPERTY(BlueprintAssignable)
	FOnTransitionStart OnTransitionStartEvent;

	// Mixer volume variables. Public so they can later be saved to a save file if that is ever needed

	UPROPERTY(BlueprintReadOnly, Category = "Mixing")
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mixing")
	float MusicVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mixing")
	float SfxVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mixing")
	float VoiceVolume = 1.0f;

	UFUNCTION()
	void OnGameOver();

	// Music playback functions

	UFUNCTION(BlueprintCallable)
	void LoadCheckpointMusic(int PlayerProgress);

	UFUNCTION(BlueprintCallable)
	void TransitionToNext(float FadeOutTime = 4.0f, float FadeInTime = 4.0f);

	UFUNCTION(BlueprintCallable)
	void SetMusic(TSubclassOf<class UMusicManager> MusicManagerClass, float FadeOutTime = 4.0f, float FadeInTime = 4.0f, bool bPersist = false, bool bPlayImmediately = false);

	UFUNCTION(BlueprintCallable)
	void StopMusic(float FadeTime = 4.0f);

	UFUNCTION(BlueprintCallable)
	void OnAddEnemy();

	UFUNCTION()
	void EventTriggered(FString EventName);

	UFUNCTION()
	void OnDialogueEnd(FString BundleName);

	UFUNCTION(BlueprintCallable)
	void PlayPersistentSFX(USoundBase* Sound);

	// Mixer volume functions

	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void SetSfxVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void SetVoiceVolume(float Volume);

	// Handles volume ducking when the player is speaking
	UFUNCTION(BlueprintCallable)
	void InputDuckVolume();

	// Returns audio back to regular volume when the player is not speaking
	UFUNCTION(BlueprintCallable)
	void InputUnduckVolume();

	UFUNCTION(BlueprintCallable)
	void ApplyConversationMix();

	UFUNCTION(BlueprintCallable)
	void ApplyCombatMix();

	UFUNCTION(BlueprintCallable)
	void ApplyPauseMix();

	UFUNCTION(BlueprintCallable)
	void RemovePauseMix();

private:
	UPROPERTY()
	bool bEventBound = false;
};
