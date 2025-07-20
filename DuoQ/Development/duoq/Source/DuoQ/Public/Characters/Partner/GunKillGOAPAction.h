// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "GunKillGOAPAction.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UGunKillGOAPAction : public UPriorityGOAPAction
{
	GENERATED_BODY()

public:
	UGunKillGOAPAction();

	UFUNCTION(BlueprintCallable)
	void BroadCastSuccess();

	UFUNCTION(BlueprintImplementableEvent)
	void HandleOnActionComplete();

	UFUNCTION(BlueprintCallable)
	void HandleStopShoot(FTimerHandle Handle, FTimerHandle EnemyHandle);

	UFUNCTION(BlueprintCallable)
	void StarlightShoot();

	UFUNCTION(BlueprintCallable)
	bool AimUpdate();
	
	void ShootTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 5.0f;
	
};
