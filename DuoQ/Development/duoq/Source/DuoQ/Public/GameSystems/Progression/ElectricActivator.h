// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameSystems/Progression/TriggerTarget.h"
#include "ElectricActivator.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API AElectricActivator : public ATriggerTarget
{
	GENERATED_BODY()

public:
	AElectricActivator();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
	float MaxCharge = 5.0f;
	
	UPROPERTY(EditAnywhere)
	float DecreaseRate = 0.5f;
	
	UPROPERTY(EditAnywhere)
	float CurrentCharge = 0.0f;
	
	UPROPERTY(EditAnywhere)
	float IncreaseRate= 5.0f;
	UPROPERTY(EditAnywhere)
	bool bIsTriggered = false;

	FTimerHandle TimerHandle;

    UPROPERTY(EditAnywhere)
    UAudioComponent* AudioComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> ActivateSFX;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> DeactivateSFX;

	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCharge(float newCharge);
};
