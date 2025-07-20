// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KeyboardAudioSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class DUOQ_API UKeyboardAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	
public:
	UFUNCTION(BlueprintCallable)
	void PlayWASDPressSound();

	UFUNCTION(BlueprintCallable)
	void PlayWASDReleaseSound();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> WASDPress;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> WASDRelease;

	UPROPERTY(EditDefaultsOnly)
	float MinMoveDistance = 250.0f;

	UPROPERTY(EditDefaultsOnly)
	int MaxConsecutiveActions = 4;

	UPROPERTY(EditDefaultsOnly)
	float Cooldown = 0.5f;

private:
	UFUNCTION()
	void ResetConsecutiveActions();

	FTimerHandle ActionCooldownTimer;

	int ConsecutiveActions = 0;
};
