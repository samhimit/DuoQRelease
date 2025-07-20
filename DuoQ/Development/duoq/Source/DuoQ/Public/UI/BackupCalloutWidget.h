// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackupCalloutWidget.generated.h"

class UCalloutHeartArrowWidget;
class UImage;
class UCanvasPanel;
class UBackupCalloutOptionWidget;
/**
 * 
 */
UCLASS()
class DUOQ_API UBackupCalloutWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void DisplayOptions();

	void HideOptions();

	UBackupCalloutOptionWidget* CalculateSelectedOption(FVector2D Direction);

	void HighlightWidget(UBackupCalloutOptionWidget* Widget);

	void UpdateHighlight(FVector2D Direction);
	void UpdateCenterDot(FVector2D Direction);

	UPROPERTY(EditDefaultsOnly)
	float CircleRadius = 200.0f;

	UPROPERTY(EditDefaultsOnly)
	float HeartRadius = 100.0f;

	void SelectHighlightedWidget(FVector2D Direction);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY()
	TArray<TObjectPtr<UBackupCalloutOptionWidget>> OptionWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UCalloutHeartArrowWidget>> HeartImages;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBackupCalloutOptionWidget> RightSideOptionWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBackupCalloutOptionWidget> LeftSideOptionWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCalloutHeartArrowWidget> HeartWidgetClass;

	TObjectPtr<UBackupCalloutOptionWidget> HighlightedWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> CenterDot;
};
