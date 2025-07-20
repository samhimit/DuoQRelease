// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditPairWidget.generated.h"

class UCanvasPanel;
class USpacer;
class UVerticalBox;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DUOQ_API UCreditPairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void FillCreditPair(FCredit Credit);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Title;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Names;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NamesRight;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NamesLeft;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> DoublePanel;
};
