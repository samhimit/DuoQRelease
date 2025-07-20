// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_RandomScoring.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UEnvQueryTest_RandomScoring : public UEnvQueryTest
{
	GENERATED_BODY()
	

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;

public:
	UEnvQueryTest_RandomScoring(const FObjectInitializer& ObjectInitializer);

};
