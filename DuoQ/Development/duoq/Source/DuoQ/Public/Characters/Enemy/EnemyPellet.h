// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Abilities/DuoQBullet.h"
#include "GameFramework/Actor.h"
#include "EnemyPellet.generated.h"

UCLASS()
class DUOQ_API AEnemyPellet : public ADuoQBullet
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyPellet();

	void SetDamage(float Damage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Pellet")
	float PelletSpeed = 100.0f;;

	UPROPERTY(EditAnywhere, Category = "Pellet")
	float PelletDamage = 50.0f;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
