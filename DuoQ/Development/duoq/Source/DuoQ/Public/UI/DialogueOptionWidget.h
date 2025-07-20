// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueOptionWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UDialogueOptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
        UFUNCTION(BlueprintCallable, Category = "UI")
        void SetOptionText(const FString& Option);

		UFUNCTION(BlueprintCallable, Category = "UI")
		void HighlightOption(FColor Color);
	
		UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void SetOptionDollarName(const FString& DollarName);

		UFUNCTION(BlueprintCallable, Category = "Dialogue")
		FString GetOptionDollarName();


    protected:
        UPROPERTY(meta = (BindWidget))
        class UTextBlock* OptionTextBlock;

		UPROPERTY(meta = (BindWidget))
		class UBorder* HighlightBorder;

		FString OptionDollarName;
};
