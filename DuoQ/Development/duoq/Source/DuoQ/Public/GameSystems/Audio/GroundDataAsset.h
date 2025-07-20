// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GroundDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UGroundDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FColor> Cache;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Width;
};
