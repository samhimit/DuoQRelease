// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UAbilityWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void SetCharge(float Percent);
};
