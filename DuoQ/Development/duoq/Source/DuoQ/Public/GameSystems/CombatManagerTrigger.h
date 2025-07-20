// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameSystems/ACombatManager.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"
#include "Progression/ColllisionTrigger.h"
#include "CombatManagerTrigger.generated.h"

UCLASS()
class DUOQ_API ACombatManagerTrigger : public ACollisionTrigger
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatManagerTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Manager")
	TArray<AGruntShotgunEnemy*> Grunts;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat Manager")
	ACombatManager* CombatManager;

	UFUNCTION(BlueprintCallable)
	void ActivateEnemies();

private:
	UFUNCTION()
	void HandlePlayerDeath();

	UFUNCTION()
	void HandlePlayerRevive();

	bool bCanActivate = true;
};
