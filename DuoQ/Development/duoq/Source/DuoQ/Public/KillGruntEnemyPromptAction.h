// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "KillGruntEnemyPromptAction.generated.h"

/**
 *
 */
UCLASS()
class DUOQ_API UKillGruntEnemyPromptAction : public UPriorityGOAPAction
{
	GENERATED_BODY()

public:
	/** the number of seconds Starlight can wait before she ask player to break the shield again*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NumSecondToWarn = 5.0f;

	/** the function that sets the timer to wait for player to shoot the shield*/
	UFUNCTION(BlueprintCallable)
	void WaitForPlayerToBreakShield();

	UFUNCTION(BlueprintCallable)
	void HandleOnShieldBroken();

	/** selecting which grunt enemy to target*/
	UFUNCTION(BlueprintCallable)
	void TargetSelection();

	UFUNCTION(BlueprintCallable)
	void SetPertinentGoalPriority(int NewPrio);

private:
	FTimerHandle WaitTimer;
};
