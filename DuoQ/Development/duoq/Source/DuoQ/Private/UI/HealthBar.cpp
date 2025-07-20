// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HealthBar.h"

#include <string>

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	if (HealthText)
	{
		HealthText->SetText(FText::FromString("100"));
	}
	
}

void UHealthBar::UpdateProgressBar(float NewHealth, float MaxHealth)
{
	if(ProgressBar && HealthText)
	{
		ProgressBar->SetPercent(NewHealth / MaxHealth);
		HealthText->SetText(FText::FromString(FString::FromInt(FMath::Clamp(ProgressBar->GetPercent() * 100, 0.0f, 100.0f))));
	}
}
