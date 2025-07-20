// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateProgressBar(float NewHealth, float MaxHealth);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> HealthText;
	
};
