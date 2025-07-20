// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VolumeSettingsWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UVolumeSettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USliderFillWidget* MasterVolume;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USliderFillWidget* ChatVolume;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USliderFillWidget* MusicVolume;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USliderFillWidget* SFXVolume;

	UFUNCTION(BlueprintCallable)
	void OnMasterValueChanged(float Value);

	UFUNCTION(BlueprintCallable)
	void OnMusicValueChanged(float Value);

	UFUNCTION(BlueprintCallable)
	void OnSFXValueChanged(float Value);

	UFUNCTION(BlueprintCallable)
	void OnChatValueChanged(float Value);

private:
	class ADuoQPlayerController* Controller;
	class UAudioSubsystem* AudioSubsystem;
	
};
