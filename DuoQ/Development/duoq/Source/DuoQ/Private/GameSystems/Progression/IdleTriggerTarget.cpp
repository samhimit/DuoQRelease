// Copyright 2024


#include "GameSystems/Progression/IdleTriggerTarget.h"

#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "GameSystems/Dialogue/DialogueTriggerComponent.h"
#include "GameSystems/Dialogue/StarlightResponseSubsystem.h"

void AIdleTriggerTarget::BeginPlay()
{
	Super::BeginPlay();
	if(UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->OnEventRaised.AddDynamic(this,&AIdleTriggerTarget::CheckDestroy);
	}
	if(UStarlightResponseSubsystem* SRS = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
	{
		SRS->OnStarlightTalkingChanged.AddDynamic(this,&AIdleTriggerTarget::ResetTimerBool);
	}
}

void AIdleTriggerTarget::Trigger(FVector source)
{
	Super::Trigger(source);
	GetWorldTimerManager().SetTimer(MyTimer,this,&AIdleTriggerTarget::FinishTrigger,DelayTime);
}

void AIdleTriggerTarget::ResetTimer()
{
	GetWorldTimerManager().SetTimer(MyTimer,this,&AIdleTriggerTarget::FinishTrigger,DelayTime);
}

void AIdleTriggerTarget::ResetTimerBool(bool bIsTalking)
{
	if(GetWorldTimerManager().IsTimerActive(MyTimer))
	{
		ResetTimer();
	}
}

void AIdleTriggerTarget::CheckDestroy(FString Event)
{
	if(Event == EndEvent)
	{
		GetWorldTimerManager().ClearTimer(MyTimer);
		Destroy();
	}
}

void AIdleTriggerTarget::OnTrigger_Implementation(FVector source)
{

}

void AIdleTriggerTarget::FinishTrigger()
{
	Super::FinishTrigger();
	DialogueTriggerComponent->TriggerDialogue();
}