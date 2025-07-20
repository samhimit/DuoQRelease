// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarPartner.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UHealthBarPartner : public UUserWidget
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
