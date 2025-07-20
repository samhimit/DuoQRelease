// Copyright 2024


#include "UI/HealthBarPartnerWidgetComponent.h"

#include "Characters/HealthComponent.h"
#include "UI/HealthBarPartner.h"

UHealthBarPartnerWidgetComponent::UHealthBarPartnerWidgetComponent()
{
	Space = EWidgetSpace::Screen;
}

void UHealthBarPartnerWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	HealthBarPartner = Cast<UHealthBarPartner>(GetUserWidgetObject());

	if (HealthBarPartner)
	{
		FVector2D Position(0.0f, 0.0f);
		HealthBarPartner->SetPositionInViewport(Position);
	}
	GetOwner()->GetComponentByClass<UHealthComponent>()->OnHealthChanged.AddDynamic(this, &UHealthBarPartnerWidgetComponent::UpdateHealthBar);
}

void UHealthBarPartnerWidgetComponent::UpdateHealthBar(float NewHealth, float MaxHealth)
{
	if (HealthBarPartner)
	{
		HealthBarPartner->UpdateProgressBar(NewHealth, MaxHealth);
	}
}
