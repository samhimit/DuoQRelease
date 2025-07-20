// Copyright 2024


#include "GameSystems/Audio/GroundData.h"

// Sets default values for this component's properties
UGroundData::UGroundData()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

FColor UGroundData::GetGroundColor(float u, float v)
{
	// Unnormalize UVs
	u *= GroundDataAsset->Width - 1;
	v *= GroundDataAsset->Width - 1;

	int iU = FMath::Clamp(FMath::RoundToInt(u), 0, GroundDataAsset->Width - 1);
	int iV = FMath::Clamp(FMath::RoundToInt(v), 0, GroundDataAsset->Width - 1);

	return GroundDataAsset->Cache[iV * GroundDataAsset->Width + iU];
}

