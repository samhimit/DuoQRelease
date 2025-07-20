// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Abilities/TP_RocketComponent.h"
#include "GameFramework/Actor.h"
#include "EnemyShotgunComponent.generated.h"

class UNiagaraSystem;

UCLASS(Blueprintable)
class DUOQ_API UEnemyShotgunComponent : public UTP_RocketComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UEnemyShotgunComponent();

	// Angle of shotgun spread cone
	UPROPERTY(EditDefaultsOnly, Category = "Shotgun");
	float ShotgunSpread = 20.0f;

	// Damage of single pellet of shotgun
	UPROPERTY(EditDefaultsOnly, Category = "Shotgun");
	float ShotgunDamage = 20.0f;

	// Amount of distance shotgun pellets should travel before disappearing
	UPROPERTY(EditDefaultsOnly, Category = "Shotgun");
	float ShotgunRange = 500.0f;

	// Number of pellets in a shotgun shot
	UPROPERTY(EditDefaultsOnly, Category = "Shotgun");
	int NumberOfPellets = 5;
	

	UFUNCTION(BlueprintCallable)
	// Spawns the pellets
	void ShootPellets();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun")
	UNiagaraSystem* EnemyBulletParticleSystem;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	

};
