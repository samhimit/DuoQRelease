// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/HealthBarWidgetComponent.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/HealthComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/HealthBar.h"

UHealthBarWidgetComponent::UHealthBarWidgetComponent()
{

	Space = EWidgetSpace::World;

}

void UHealthBarWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{

		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), Character->GetActorLocation());
		Rotation.Pitch = 0.0f;
		Rotation.Roll = 0.0f;
		SetWorldRotation(Rotation.Quaternion());

	}
}

void UHealthBarWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	HealthBar = Cast<UHealthBar>(GetUserWidgetObject());
	GetOwner()->GetComponentByClass<UHealthComponent>()->OnHealthChanged.AddDynamic(this, &UHealthBarWidgetComponent::UpdateHealthBar);

}

void UHealthBarWidgetComponent::UpdateHealthBar(float NewHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->UpdateProgressBar(NewHealth, MaxHealth);
	}
}
