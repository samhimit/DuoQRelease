// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "DelayedTriggerTarget.h"
#include "GameSystems/Progression/TriggerTarget.h"
#include "IdleTriggerTarget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API AIdleTriggerTarget : public ADelayedTriggerTarget
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void Trigger(FVector source) override;

	UFUNCTION()
	void ResetTimer();

	UFUNCTION()
	void ResetTimerBool(bool bIsTalking);

	UPROPERTY(EditInstanceOnly)
	FString EndEvent;

	UFUNCTION()
	void CheckDestroy(FString Event);

	virtual void OnTrigger_Implementation(FVector source) override;

	virtual void FinishTrigger() override;
};
