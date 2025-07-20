// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TempMagnetVortexHUD.h"
#include "PlayerMagnetVortex.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVortex);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UPlayerMagnetVortex : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerMagnetVortex();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	float MaxCharge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	float CurrentCharge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	float UseChargeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	float RechargeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	float MaxPlacementRange;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnVortex OnVortex;


	// Reference to the indicator actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	AActor* IndicatorActor;

	// The class type of the indicator actor to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	TSubclassOf<AActor> IndicatorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vortex Ability")
	TSubclassOf<AActor> IndicatorPlaced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* SelectSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ActivationSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FComponentReference LoopSfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* AttractSfx;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MagneticVortexAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CancelPlacementAction;


	// Whether the ability is active
	bool bIsAbilityActive;

	// Whether the player is holding the ability key
	bool bIsHoldingKey;

	// Indicator location
	FVector IndicatorLocation;

	UFUNCTION(BlueprintCallable, Category = "Vortex Ability")
	void SpawnMagnetVortex();

	UFUNCTION(BlueprintCallable, Category = "Vortex Ability")
	void DespawnMagnetVortex();

	void Recharge(float DeltaTime);

	void DrainCharge(float DeltaTime);

	// Input bindings
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	void CancelPlacement();

protected:

	// Whether the player is in placement mode
	bool bIsInPlacementMode;
	// Boolean to track if the indicator is placed
	bool bIndicatorPlaced;
	// Functions
	void ApplyStun(FVector IndicatorLocation);

	float OriginalWalkSpeed;

	FVector GetAbilityPlacementLocation();
	void	PlaceIndicator();
};
