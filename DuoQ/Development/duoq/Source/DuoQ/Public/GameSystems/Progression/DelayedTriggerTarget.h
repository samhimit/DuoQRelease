// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameSystems/Progression/TriggerTarget.h"
#include "DelayedTriggerTarget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API ADelayedTriggerTarget : public ATriggerTarget
{
	GENERATED_BODY()

	virtual void ActivateTrigger(FVector source) override;
	
protected:
	UFUNCTION()
	virtual void FinishTrigger();
};
