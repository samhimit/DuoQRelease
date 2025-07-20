// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SAMWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API USAMWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void UpdateSAMUI(int ScoreChange, bool bChangedLevels);
};
