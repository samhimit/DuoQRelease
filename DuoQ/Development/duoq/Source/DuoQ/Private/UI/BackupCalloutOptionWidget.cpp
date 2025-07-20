// Copyright 2024


#include "UI/BackupCalloutOptionWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/CalloutHeartArrowWidget.h"

void UBackupCalloutOptionWidget::SetHighlight(FColor BackgroundColor, FColor TextColor)
{
	if(OptionText)
	{
		OptionText->SetColorAndOpacity(TextColor);
	}
	if(HighlightBorder)
	{
		HighlightBorder->SetBrushColor(BackgroundColor);
	}
	if(OptionArrow)
	{
		OptionArrow->HeartImage->SetBrushTintColor(BackgroundColor);
	}
}