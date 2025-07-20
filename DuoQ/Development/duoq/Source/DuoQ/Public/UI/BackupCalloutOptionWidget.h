// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "Blueprint/UserWidget.h"
#include "BackupCalloutOptionWidget.generated.h"

class UCalloutHeartArrowWidget;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DUOQ_API UBackupCalloutOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionText;

	FAtom Goal;

	FString CalloutText;

	UPROPERTY(meta = (BindWidget))
	class UBorder* HighlightBorder;

	TObjectPtr<UCalloutHeartArrowWidget> OptionArrow;

	void SetHighlight(FColor BackgroundColor, FColor TextColor);
};
