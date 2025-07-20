// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "Characters/DuoQCharacter.h"
#include "DuoQEnemyCharacter.generated.h"

class UEnemyAimPoint;
class UNiagaraSystem;
class UPriorityGOAPAction;
/**
 * 
 */
UCLASS()
class DUOQ_API ADuoQEnemyCharacter : public ADuoQCharacter
{
	GENERATED_BODY()
public:
	ADuoQEnemyCharacter();

	UFUNCTION(BlueprintCallable)
	void GenerateGOAPActions();

	UFUNCTION(BlueprintCallable)
	void RemoveGOAPActions(AActor* Actor);

	/** returns a target actor on the enemy fofr Starlight to shoot at*/
	UFUNCTION(BlueprintCallable)
	AActor* ReturnValidTarget();

	void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Override the HandleDeath function
	UFUNCTION(BlueprintCallable)
	void HandleDeath() override;

	UPROPERTY(BlueprintReadOnly)
	FString EnemyAliveStateName;
	UPROPERTY(BlueprintReadOnly)
	FString EnemyInSightStateName;

	FString EnemyID;

	UPROPERTY(BlueprintReadWrite)
	bool bHasBeenSeen = false;

	UFUNCTION()
	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int KillGoalPriority = 1;

	/** this function generates kill goal autimatically when this enemy sees the player or Starlight*/
	UFUNCTION(BlueprintCallable)
	void HandleOnSeenStarlightOrPlayer(bool bIsInSight);

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Effects")
	// UNiagaraSystem* RegularDamageParticleSystem;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Effects")
	// UNiagaraSystem* ElectricDamageParticleSystem;

	// Aim Point for Starlight to shoot at
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UEnemyAimPoint* AimPoint;

	UPROPERTY()
	bool bIsDead = false;
	
	
protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<UPriorityGOAPAction> KillActionClass;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<UPriorityGOAPAction> MoveToActionClass;

	UPROPERTY()
	FName EnemyKillActionName;
	UPROPERTY()
	FName EnemyMoveActionName;
};
