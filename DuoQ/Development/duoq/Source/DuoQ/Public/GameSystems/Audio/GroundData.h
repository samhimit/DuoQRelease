// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GroundDataAsset.h"
#include "GroundData.generated.h"

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUOQ_API UGroundData : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGroundData();

	UFUNCTION(BlueprintCallable)
	FColor GetGroundColor(float u, float v);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGroundDataAsset> GroundDataAsset;
};
