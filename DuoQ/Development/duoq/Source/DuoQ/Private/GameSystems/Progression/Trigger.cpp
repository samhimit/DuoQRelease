// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/Progression/Trigger.h"
#include "GameSystems/Progression/TriggerTarget.h"

// Sets default values
ATrigger::ATrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATrigger::BeginPlay()
{
	Super::BeginPlay();
}
void ATrigger::DeactivateTrigger()
{
	if (!IsValid(this))
	{
		return;
	}
	for (auto target : MyTargets)
	{
		ATriggerTarget* t = target.Get();
		if (t != nullptr)
		{
			t->UnTrigger();
		}
	}
	if (bNeedOnDeactivated)
	{
		OnDeactivated();
	}
}
void ATrigger::DoTriggerWithSource(FVector source)
{
	for (auto target : MyTargets)
	{
		ATriggerTarget* t = target.Get();
		if (t != nullptr)
		{
			t->Trigger(source);
		}
	}
}
void ATrigger::ActivateTrigger()
{
	for (auto target : MyTargets)
	{
		ATriggerTarget* t = target.Get();
		if (t != nullptr)
		{
			t->Trigger();
		}
	}
	OnActivated(GetActorLocation());
}
void ATrigger::ActivateTrigger(FVector source)
{
	for (auto target : MyTargets)
	{
		ATriggerTarget* t = target.Get();
		if (t != nullptr)
		{
			t->Trigger(source);
		}
	}

	// check if we want the trigger to be delayed
	/* if (DelayTime != 0.0f)
	{
		MyTimerDelegate.BindUObject(this, &ATrigger::DoTriggerWithSource, source);
		GetWorldTimerManager().SetTimer(MyTimer, MyTimerDelegate, DelayTime, false);
	}
	else // trigger is not delayed
	{
		DoTriggerWithSource(source);
	}*/
	OnActivated(source);
}