// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SliderFillWidget.generated.h"

class USlider;
class UProgressBar;
/**
 * 
 */
UCLASS()
class DUOQ_API USliderFillWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* Slider;
	
};
