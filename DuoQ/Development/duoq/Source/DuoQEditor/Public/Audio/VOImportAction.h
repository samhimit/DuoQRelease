// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Actions/IRActionBase.h"
#include "VOImportAction.generated.h"

/**
 *
 */
UCLASS()
class DUOQEDITOR_API UVOImportAction : public UIRActionBase
{
	GENERATED_BODY()

public:
	virtual bool Apply_Implementation(UFactory* Factory, UObject* CreatedObject) override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundClass> VoiceClass;

	UPROPERTY(EditAnywhere)
	TArray<FSoundSourceBusSendInfo> PreEffectBusSends;

	UPROPERTY(EditAnywhere)
	TArray<FSoundSourceBusSendInfo> BusSends;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundEffectSourcePresetChain> VoiceFX;

private:
	bool SetVoiceOver(class USoundWave* sound, FString tableName, FString dollarName);
	bool SetVoiceOver(class USoundWave* sound, FString dollarName); // Old naming convention
};
