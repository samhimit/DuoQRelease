// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DUOQ_API UHealthBarWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UHealthBarWidgetComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateHealthBar(float NewHealth, float MaxHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UHealthBar> HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UHealthBarPartner> HealthBarPartner;
	
};
