// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "DuoQGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UDuoQGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UDuoQGameUserSettings();
	
	UFUNCTION(BlueprintPure)
	bool GetShouldRunHardwareBenchmark() const { return bShouldRunHardwareBenchmark; }
	UFUNCTION(BlueprintCallable)
	void SetShouldRunHardwareBenchmark(bool bNewValue);

	UFUNCTION(BlueprintCallable)
	static UDuoQGameUserSettings* GetDuoQGameUserSettings();
	
protected:
	UPROPERTY(Config)
	bool bShouldRunHardwareBenchmark;
};
