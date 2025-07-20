// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueOptionWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "DialogueWidget.generated.h"

struct FDollar;
class UImage;
class UDialogueOptionWidgetFlipped;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class DUOQ_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void ShowSubtitle(const FString& Subtitle);
	
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void HideSubtitle();

	UFUNCTION(BlueprintCallable)
	void ShowOptions(const TArray<FDollar>& Dollars, bool bOpenEnded);

	UFUNCTION(BlueprintCallable)
	void DelayShowOptions(const TArray<FDollar>& Dollars, bool bOpenEnded);

	UFUNCTION(BlueprintCallable)
	void SetHiddenOptionsVisibility(bool bIsVisible);
	
	UFUNCTION(BlueprintCallable)
	void HideOptions();

	void UpdateHighlight(FVector2D Direction);
	void UpdateCenterDot(FVector2D Direction);

	void HoverOption(UDialogueOptionWidget* NewHoveredWidget);

	UDialogueOptionWidget* CalculateSelectedOption(FVector2D Direction);

	// Highlight the option based on its DollarName
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void HighlightOption(const FString& DollarName);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFadeOptions3(bool FadeOut);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFadeOptions2(bool FadeOut);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFadeOptions1(bool FadeOut);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFadeCenterDot(bool FadeOut);

	UFUNCTION(BlueprintImplementableEvent)
	void RevertStarlight();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float OptionsVerticalMargin = 10.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float OptionsHorizontalMargin = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MaxDialogueOptions = 4;

	//UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	//TObjectPtr<UTextBlock> SaySomethingText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UVerticalBox> OptionsLeftVerticalBox;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UVerticalBox> OptionsRightVerticalBox;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> OptionsHorizontalBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> CenterDot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UDialogueOptionWidget> OpenEndedPrompt;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UDialogueOptionWidget> HiddenOptionReveal;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Dialogue")
	TSubclassOf<UDialogueOptionWidget> OptionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Dialogue")
	TSubclassOf<UDialogueOptionWidgetFlipped> OptionWidgetFlippedClass;

	UPROPERTY()
	TArray<UDialogueOptionWidget*> OptionWidgets;

	UPROPERTY()
	TArray<FDollar> HiddenOptionWidgets;

	UPROPERTY()
	TObjectPtr<UDialogueOptionWidget> HighlightedWidget = nullptr;

	bool bIsInOpenEnded = false;
};
