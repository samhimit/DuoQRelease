// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MouseSensitivityWidget.generated.h"

class UTextBlock;
class USliderFillWidget;
class ADuoQPlayerController;
class USlider;
/**
 * 
 */
UCLASS()
class DUOQ_API UMouseSensitivityWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USliderFillWidget* SensitivitySlider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* Text;

	UFUNCTION(BlueprintCallable)
	void OnSensitivityChanged(float Value);

	UPROPERTY(EditDefaultsOnly, Category = "Mouse Sensitivity")
	float MouseSensitivity = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Mouse Sensitivity")
	float MouseSensitivityMultiplier = 1.0f;

protected:
	virtual void NativeConstruct() override;

	ADuoQPlayerController* Controller;
	
};
