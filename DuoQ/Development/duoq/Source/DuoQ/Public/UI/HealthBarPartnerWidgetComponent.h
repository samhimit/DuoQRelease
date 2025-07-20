// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarPartnerWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UHealthBarPartnerWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UHealthBarPartnerWidgetComponent();
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateHealthBar(float NewHealth, float MaxHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UHealthBarPartner> HealthBarPartner;
	
};
