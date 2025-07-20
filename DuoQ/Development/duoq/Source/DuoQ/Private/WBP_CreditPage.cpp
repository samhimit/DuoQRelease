// Copyright 2024


#include "WBP_CreditPage.h"

#include "CreditPairWidget.h"
#include "GameSystems/Progression/CreditsSubsystem.h"

void UWBP_CreditPage::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWBP_CreditPage::FillCredits(FCreditPage CreditPage)
{
	for(int i = 0; i < CreditPage.Credits.Num(); i++)
	{
		CreditPairs[i]->FillCreditPair(CreditPage.Credits[i]);
	}
}