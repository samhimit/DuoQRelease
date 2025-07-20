// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StarlightResponseSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStarlightTalkingChanged, bool, bStarlightIsTalking);

/**
 * 
 */
UCLASS()
class DUOQ_API UStarlightResponseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
public:
	UFUNCTION(BlueprintCallable)
	void HandleCallout(USoundBase* VoiceOver, FString Subtitle);

	UFUNCTION(BlueprintCallable)
	void HandleDialogue(FString& LineName, FString& Subtitle, USoundBase* VoiceOver);

	UFUNCTION()
	void HideDialogue();

	UFUNCTION(BlueprintCallable)
	void OnPlayerResponseTimeout();

	UFUNCTION()
	void SetStarlightTalking(bool bIsTalking);

	UFUNCTION(BlueprintCallable)
	bool GetStarlightTalking();

	FTimerHandle PlayerResponseTimer;

	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FOnStarlightTalkingChanged OnStarlightTalkingChanged;

	bool bIsStarlight = false;

protected:
	float DialogueShowTime = 0.0f;
	float TempDialogueShowTime = 7.0f;
	
	float PlayerResponseTime = 7.0f;
	
private:
	FTimerHandle DialogueTimer;
	bool bIsStarlightTalking;

	UPROPERTY()
	UAudioComponent* VoiceOverSound;
};
