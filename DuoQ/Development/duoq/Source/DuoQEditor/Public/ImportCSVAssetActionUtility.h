// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "ImportCSVAssetActionUtility.generated.h"

class UVaRestRequestJSON;
/**
 * 
 */
UCLASS()
class DUOQEDITOR_API UImportCSVAssetActionUtility : public UAssetActionUtility
{
	GENERATED_BODY()

	UImportCSVAssetActionUtility();

public:
	UFUNCTION(CallInEditor)
	void ImportCSVs();

	UFUNCTION(CallInEditor,BlueprintCallable)
	void ImportSingleSheet(UDataTable* DataTable);

protected:
	UFUNCTION()
	virtual void CompleteSheetIDRequest(class UVaRestRequestJSON* Request);
	
	UFUNCTION()
	virtual void CompleteCSVInfoRequest(class UVaRestRequestJSON* Request);
	
	UFUNCTION()
	virtual void FailRequest(class UVaRestRequestJSON* Request);

	UFUNCTION()
	void SetVoiceLines(UDataTable* DataTable);

	UFUNCTION()
	void InitializeVO();

	UPROPERTY()
	TMap<int,FString> TableNames;

	UPROPERTY()
	TMap<FName, USoundBase*> VOAssets;
};
