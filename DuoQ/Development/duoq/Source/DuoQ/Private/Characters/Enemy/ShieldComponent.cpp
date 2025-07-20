// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/ShieldComponent.h"

UShieldComponent::UShieldComponent()
{
	
}

void UShieldComponent::HandleDamage(float Damage, USceneComponent* HitComponent, bool bApplyMultipliers, AActor* DamageCauser)
{
	if(!bIsShieldActive)
	{
		Super::HandleDamage(Damage, HitComponent, bApplyMultipliers);
	}
}

void UShieldComponent::ApplyStun()
{
	if (bIsShieldActive)
	{
		bIsShieldActive = false;
		OnBreak.Broadcast();
		//SetActive(false);
		GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &UShieldComponent::EnableShield, RechargeTime, false);
	}
	else
	{
		Super::ApplyStun();
	}
	
}

void UShieldComponent::EnableShield()
{
	//SetActive(true);
	bIsShieldActive = true;
	OnRegen.Broadcast();
}
