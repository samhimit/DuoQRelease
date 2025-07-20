// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "GroundDataAssetActionUtility.generated.h"

/**
 * 
 */
UCLASS()
class DUOQEDITOR_API UGroundDataAssetActionUtility : public UAssetActionUtility
{
	GENERATED_BODY()

	UGroundDataAssetActionUtility();
	
public:
	UFUNCTION(CallInEditor)
	virtual void CacheVertexColorTexture(class UTexture2D* VertexColorTexture);
};
