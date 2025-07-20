// Copyright 2024


#include "GameSystems/Progression/DelayedTriggerTarget.h"

void ADelayedTriggerTarget::ActivateTrigger(FVector source)
{
	GetWorldTimerManager().SetTimer(MyTimer,this,&ADelayedTriggerTarget::FinishTrigger,DelayTime);
}

void ADelayedTriggerTarget::FinishTrigger()
{
	Super::ActivateTrigger(GetActorLocation());
}