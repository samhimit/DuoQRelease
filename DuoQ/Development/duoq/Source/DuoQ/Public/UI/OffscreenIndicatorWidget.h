// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OffscreenIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UOffscreenIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* IndicatorText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SpeakerRight;
	
	UFUNCTION(BlueprintCallable, Category = "Offscreen Indicator")
	void SetIndicatorPosition(FVector2D Position, bool bShouldBeVisible);

	class UTextBlock* GetIndicatorText() const { return IndicatorText; }

	bool bShouldBeHiddenPermanently = false;

protected:
	virtual void NativeConstruct() override;

	int32 GetViewportX();
};
