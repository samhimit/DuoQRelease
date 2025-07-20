// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityWidget.h"
#include "GunChargeWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UGunChargeWidget : public UAbilityWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void HideChargeUI();
};
