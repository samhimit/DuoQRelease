// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/HealthComponent.h"
#include "ShieldComponent.generated.h"

// Delegates for shield break and regen
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBreak);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegen);

UCLASS(Blueprintable)
class DUOQ_API UShieldComponent : public UHealthComponent
{
	GENERATED_BODY()

public:
	UShieldComponent();

	// function to handle damage taken
	virtual void HandleDamage(float Damage, USceneComponent* HitComponent, bool bApplyMultipliers, AActor* DamageCauser = nullptr) override;
	
	virtual void ApplyStun() override;

	// Delegate for shield breaking
	UPROPERTY(BlueprintAssignable, Category = "Shield Component")
	FOnBreak OnBreak;

	// Delegate for shield regenerating
	UPROPERTY(BlueprintAssignable, Category = "Shield Component")
	FOnRegen OnRegen;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Shield Component")
	bool bIsShieldActive = true;

	FTimerHandle RegenTimerHandle;

	void EnableShield();
	
};
