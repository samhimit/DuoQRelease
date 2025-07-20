// Copyright 2024


#include "CreditsWidget.h"

#include "WBP_CreditPage.h"

void UCreditsWidget::ClearCredits()
{
	CreditPages.Empty();
}

UWBP_CreditPage* UCreditsWidget::GetCreditPage(int i)
{
	if(auto CreditPage = CreditPages.Find(i))
	{
		if(!CreditPage->IsEmpty())
		{
			int j = FMath::RandRange(0,CreditPage->Num()-1);
			return (*CreditPage)[j];
		}
	}
	return nullptr;
}

void UCreditsWidget::AddCreditPage(UWBP_CreditPage* Page)
{
	if(Page)
	{
		CreditPages.FindOrAdd(Page->CreditPairs.Num()).Add(Page);
		UE_LOG(LogTemp, Warning, TEXT("%i"), Page->CreditPairs.Num());
	}
}