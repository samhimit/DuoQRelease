// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "ChooseAreaGOAPAction.generated.h"

/**
 *
 */
UCLASS()
class DUOQ_API UChooseAreaGOAPAction : public UPriorityGOAPAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool FindNextArea();

	UFUNCTION(BlueprintCallable)
	void SetPertinentGoalPriority(int NewPrio);
};
