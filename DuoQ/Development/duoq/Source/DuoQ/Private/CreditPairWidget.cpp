// Copyright 2024


#include "CreditPairWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "GameSystems/Progression/CreditsSubsystem.h"

void UCreditPairWidget::FillCreditPair(FCredit Credit)
{
	if(Title)
	{
		Title->SetText(Credit.Title);
	}
	if(Names && NamesRight)
	{
		if(Credit.Names.Num() == 1 || Credit.Names.Num() == 3 || Credit.Title.EqualTo(FText::FromString(TEXT("Cast"))))
		{
			FText Combined = FText::FromString(FString::Join(Credit.Names,TEXT("\n")));
			Names->SetText(Combined);
			DoublePanel->SetVisibility(ESlateVisibility::Collapsed);
			Names->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			DoublePanel->SetVisibility(ESlateVisibility::Visible);
			Names->SetVisibility(ESlateVisibility::Collapsed);
			TArray<FString> NamesRightArray;
			TArray<FString> NamesLeftArray;
			for(int i = 0; i < Credit.Names.Num(); i++)
			{
				if(i % 2 == 0)
				{
					
					NamesLeftArray.Add(Credit.Names[i]);
				}
				else
				{
					NamesRightArray.Add(Credit.Names[i]);
				}
			}
			FText CombinedRight = FText::FromString(FString::Join(NamesRightArray,TEXT("\n")));
			NamesRight->SetText(CombinedRight);
			FText CombinedLeft = FText::FromString(FString::Join(NamesLeftArray,TEXT("\n")));
			NamesLeft->SetText(CombinedLeft);
		}
	}
}