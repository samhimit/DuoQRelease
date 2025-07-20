// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyShotgun.generated.h"

UCLASS()
class DUOQ_API AEnemyShotgun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyShotgun();

	UFUNCTION(BlueprintCallable, Category = "Shotgun")
	void Fire();

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Spawns the pellets
	void ShootPellet(const FVector& SpawnLocation, const FRotator& SpawnRotation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
