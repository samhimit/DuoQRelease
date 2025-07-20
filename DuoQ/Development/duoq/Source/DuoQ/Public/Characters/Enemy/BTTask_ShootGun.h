// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ShootGun.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UBTTask_ShootGun : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ShootGun();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
