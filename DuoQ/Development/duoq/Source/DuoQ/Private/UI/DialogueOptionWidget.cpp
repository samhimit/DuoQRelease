// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DialogueOptionWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"


void UDialogueOptionWidget::SetOptionText(const FString& Option)
{
	if(OptionTextBlock)
	{
		OptionTextBlock->SetText(FText::FromString(Option));
	}
}

void UDialogueOptionWidget::HighlightOption(FColor Color)
{
	if(HighlightBorder)
	{
		HighlightBorder->SetBrushColor(Color);
	}
}

void UDialogueOptionWidget::SetOptionDollarName(const FString& DollarName)
{
	OptionDollarName = DollarName;
}

FString UDialogueOptionWidget::GetOptionDollarName()
{
	return OptionDollarName;
}