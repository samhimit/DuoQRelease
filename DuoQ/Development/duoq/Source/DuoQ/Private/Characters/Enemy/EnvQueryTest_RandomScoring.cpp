// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnvQueryTest_RandomScoring.h"

#include "AISystem.h"
#include "EnvironmentQuery/Tests/EnvQueryTest_Random.h"

UEnvQueryTest_RandomScoring::UEnvQueryTest_RandomScoring(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType::StaticClass();
}

void UEnvQueryTest_RandomScoring::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	for(FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		It.SetScore(TestPurpose, FilterType, UAISystem::GetRandomStream().FRand(), FloatValueMin.GetValue(), FloatValueMax.GetValue());
	}
	
}

FText UEnvQueryTest_RandomScoring::GetDescriptionTitle() const
{
	return FText::FromString("Random");
}

FText UEnvQueryTest_RandomScoring::GetDescriptionDetails() const
{
	return Super::GetDescriptionDetails();
}
