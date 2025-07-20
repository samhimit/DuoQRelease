// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TurretAIController.generated.h"

/**
 * 
 */

class ATurretEnemy;

UCLASS()
class DUOQ_API ATurretAIController : public AAIController
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	AActor* FindClosestToLOSTarget();

protected:

	UPROPERTY(EditDefaultsOnly)
	TArray<AActor*> TargetActors;

	UPROPERTY(EditDefaultsOnly)
	TArray<AActor*> CurrentActor;

	UPROPERTY(EditDefaultsOnly)
	ATurretEnemy* Turret;

	UPROPERTY(EditDefaultsOnly)
	bool bSpotted;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* BehaviorTree;
	
};
