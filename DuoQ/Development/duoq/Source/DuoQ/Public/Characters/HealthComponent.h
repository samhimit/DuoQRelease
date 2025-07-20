// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "TimerManager.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"
#include "HealthComponent.generated.h"


// Declare a delegate type for the death event
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathWithSource, FVector, Source);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathWithKiller, AActor*, Killer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLowHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGruntHealthChanged, AGruntShotgunEnemy*, OwningGrunt, float, NewHealth);


//Daclare delegates for shield events
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldBreak);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldRecharge);

//Declare delegates when stun is applied and removed
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemoveStun);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUOQ_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Constructor
	UHealthComponent();

	// Maximum health value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	// Current health value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

	// Delay before health regeneration starts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float RegenDelay;

	// Rate at which health regenerates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float RegenRate;

	//Time stun lasts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float StunDuration;

	// Flag to determine if health can regenerate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bCanRegen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	bool bHasShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	bool bShielded;

	// Flag to determine if the actor is friendly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool IsFriendly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* ImpactVFX; 

	// Delegate for handling death
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathWithSource OnDeathWithSource;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathWithKiller OnDeathWithKiller;

	// Delegate for low health warning
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLowHealth OnLowHealth;

	// Delegate for handling health changes
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGruntHealthChanged OnGruntHealthChanged;

	//Delegates related to Stun
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStun OnStun;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRemoveStun OnRemoveStun;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnShieldBreak OnShieldBreak;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnShieldRecharge OnShieldRecharge;

	UPROPERTY(EditDefaultsOnly, Category = "Shield Component")
	float RechargeTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float LowHealthRatio = 0.1f;

	// Map to store scene components and their damage multipliers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	TMap<USceneComponent*, float> DamageMultipliers;

	// Container for status effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	FGameplayTagContainer ActiveStatusEffects;

	// Function to add a component to the map with a damage multiplier
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddDamageMultiplier(USceneComponent* Component, float Multiplier);

	// Function to add a status effect
	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	void AddStatusEffect(FGameplayTag StatusEffect);

	// Function to remove a status effect
	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	void RemoveStatusEffect(FGameplayTag StatusEffect);

	UFUNCTION(BlueprintCallable)
	bool IsDead() { return bIsDead; }

	// Function to check if a specific status effect is active
	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	bool HasStatusEffect(FGameplayTag StatusEffect) const;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void HandleDamage(float Damage, USceneComponent* HitComponent = nullptr, bool bApplyMultipliers = true, AActor* DamageCauser = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Revive();


	// Function to check if another HealthComponent is friendly
	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsFriendlyWith(const UHealthComponent* OthberHealthComponent) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	USoundBase* StunStartSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	FComponentReference StunLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	USoundBase* StunEndSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* LowHealthSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FRuntimeFloatCurve LowHealthLoopGain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* LowHealthLoopSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* HealthRegenSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* HealthRegenStopSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* HurtSfx;


protected:

	FGameplayTag StunTag;

	// Timer handle for regeneration (private)
	FTimerHandle RegenTimerHandle;

	FTimerHandle ShieldRegenTimerHandle;

	//Timer handle for stun
	FTimerHandle StunTimerHandle;

	FTimerHandle ReviveRegenSoundTimerHandle;

	//Is player currently regening or not
	UPROPERTY(BlueprintReadOnly)
	bool bIsRegening;

	// Called when player hasn't taken damage after a certain time
	void HandleRegeneration();

	void TickRegeneration(float DeltaTime);

	// Called when the game starts
	virtual void BeginPlay() override;

	//Called when actor has taken a stun effetct
	virtual void ApplyStun();

	//Called when stun timer is up
	void RemoveStun();

	// Handle point damage event
	UFUNCTION()
	void HandleTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	// Handle taking radial damage
	UFUNCTION()
	void HandleTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector HitLocation, FHitResult const& HitInfo, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void HandleDeath();

	

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY()
	bool bIsDead;

	UFUNCTION()
	void ApplyShield();

	UFUNCTION()
	void ResetRegenSound();

	UPROPERTY()
	bool bCanPlayRegenSound = true;

	UAudioComponent* Audio;
};
