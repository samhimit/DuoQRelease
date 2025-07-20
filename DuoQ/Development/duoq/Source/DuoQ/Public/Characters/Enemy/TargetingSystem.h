// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingSystem.generated.h"


class ADuoQEnemyCharacter;
class ADuoQCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UTargetingSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingSystem();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	ADuoQCharacter* GetTarget(ADuoQEnemyCharacter* Enemy);

	void AddEnemyShootingAt(ADuoQEnemyCharacter* Enemy, ADuoQCharacter* Target);
	void RemoveEnemyShootingAt(ADuoQEnemyCharacter* Enemy);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	bool IsInLineOfSight(const ADuoQCharacter* Target) const;
	float DistanceToTarget(const ADuoQCharacter* Target) const;
	ADuoQCharacter* DetermineTarget(ADuoQEnemyCharacter* Enemy);

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float MaxDistance = 1000.0f;

	UPROPERTY()
	TArray<ADuoQEnemyCharacter*> EnemiesShootingAtPlayer;

	UPROPERTY()
	TArray<ADuoQEnemyCharacter*> EnemiesShootingAtStarlight;

	ADuoQCharacter* CurrentTarget = nullptr;
	ADuoQCharacter* OldTarget = nullptr;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// function to clean up enemies shooting at player/starlight
	void CleanupTargets();
		
};
