// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "MoveToCoverAction.generated.h"

/**
 *
 */
UCLASS()
class DUOQ_API UMoveToCoverAction : public UPriorityGOAPAction
{
	GENERATED_BODY()

public:


	void ResetTimer();

private:
	FTimerHandle WarnHandle;
};
