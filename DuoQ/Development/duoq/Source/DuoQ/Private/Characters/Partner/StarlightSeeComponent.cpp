// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Partner/StarlightSeeComponent.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Partner/StarlightGOAPController.h"


// Sets default values for this component's properties
UStarlightSeeComponent::UStarlightSeeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStarlightSeeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bShouldAddToTagList)
	{
		if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
		{
			SGC->ActorsWithTags.Add(GetOwner());
		}
	}

	// ...
	
}


// Called every frame
void UStarlightSeeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

