// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "FireRocketGOAPAction.generated.h"

/**
 *
 */
UCLASS()
class DUOQ_API UFireRocketGOAPAction : public UPriorityGOAPAction
{
	GENERATED_BODY()

public:
	/** used to check whether the player is in front. Returns true if the player is infront and starlight should strafe
	 *	false if they player is behind and starlight should say something
	 */
	UFUNCTION(BlueprintCallable)
	bool CheckPlayerStatus();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WarnWaitTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerInPosition = false;

	void ResetWarningTimer();

private:
	FTimerHandle WarnHandle;
};
