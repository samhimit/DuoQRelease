// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MetricSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UMetricSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	

public:
	void TrackFailedDollar(FString Dollar, FString UserMessage);

protected:
	FString Time;
	FString DialoguePathName;
	FString GOAPPathName;
	FString FailedDollarPathName;

	TMap<FString,int> FailedDollars;
	
};
