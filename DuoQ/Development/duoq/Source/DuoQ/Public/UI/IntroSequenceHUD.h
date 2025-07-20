// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntroSequenceHUD.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UIntroSequenceHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UDialogueWidget> DialogueHUD;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UControlsWidget> ControlsWidget;
};
