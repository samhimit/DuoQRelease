// Copyright 2024


#include "UI/HealthBarPartner.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBarPartner::NativeConstruct()
{
	Super::NativeConstruct();

	if (HealthText)
	{
		HealthText->SetText(FText::FromString("100"));
	}
}

void UHealthBarPartner::UpdateProgressBar(float NewHealth, float MaxHealth)
{
	if(ProgressBar && HealthText)
	{
		ProgressBar->SetPercent(NewHealth / MaxHealth);
		HealthText->SetText(FText::FromString(FString::FromInt(FMath::Clamp(ProgressBar->GetPercent() * 100, 0.0f, 100.0f))));
		
	}
}
