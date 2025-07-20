// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RotateGun.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UBTTask_RotateGun : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_RotateGun();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Rotate")
	float RotationSpeed = 50.0f;
	
};
