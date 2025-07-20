// Copyright 2024


#include "Audio/GroundDataAssetActionUtility.h"
#include "EditorUtilityLibrary.h"
#include "GameSystems/Audio/GroundDataAsset.h"

UGroundDataAssetActionUtility::UGroundDataAssetActionUtility()
{
	SupportedClasses.Emplace(UGroundDataAsset::StaticClass());
}

void UGroundDataAssetActionUtility::CacheVertexColorTexture(UTexture2D* VertexColorTexture)
{
	TArray<UObject*> Selections = UEditorUtilityLibrary::GetSelectedAssets();

	for (UObject* Selection : Selections)
	{
		if (UGroundDataAsset* GroundData = Cast<UGroundDataAsset>(Selection))
		{
			GroundData->Width = VertexColorTexture->GetSizeX();
			GroundData->Cache.SetNumUninitialized(GroundData->Width * GroundData->Width);

			const FColor* Pixels1 = static_cast<const FColor*>(VertexColorTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_ONLY));
			for (int32 X = 0; X < GroundData->Width; X++)
			{
				for (int32 Y = 0; Y < GroundData->Width; Y++)
				{
					GroundData->Cache[Y * GroundData->Width + X] = Pixels1[Y * GroundData->Width + X];
				}
			}
			VertexColorTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
			Selection->Modify();
		}
	}
}