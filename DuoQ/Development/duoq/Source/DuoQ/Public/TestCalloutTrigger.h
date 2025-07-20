// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TestCalloutTrigger.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API ATestCalloutTrigger : public ATriggerBox
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FString CalloutTableName;
	
	UFUNCTION()
	void OnPlayerEnterTriggerBox(AActor* OverlappedActor, AActor* OtherActor);
};
