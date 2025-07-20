// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/Progression/TriggerTarget.h"
#include "GameSystems/Progression/Trigger.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "Characters/HealthComponent.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "GameSystems/Dialogue/DialogueTriggerComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"

// Sets default values
ATriggerTarget::ATriggerTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DialogueTriggerComponent = CreateDefaultSubobject<UDialogueTriggerComponent>(TEXT("DialogueTriggerComponent"));
}

// Called when the game starts or when spawned
void ATriggerTarget::BeginPlay()
{
	AActor::BeginPlay();
	CurrentCount = 0;
	for (auto t : MyTriggers)
	{
		ATrigger* trigger = t.Get();
		if (trigger != nullptr)
		{
			trigger->AddTarget(TSoftObjectPtr<ATriggerTarget>(this));
			if (ATriggerTarget* TriggerTarget = Cast<ATriggerTarget>(trigger); TriggerTarget && TriggerTarget->GetTriggerMode() == ETriggerMode::NOTRELAY)
			{
				Trigger();
			}
		}
	}
	switch (MyTriggerMode)
	{
		case ETriggerMode::AND:
			Threshold = MyTriggers.Num() + MyEnemyTriggers.Num() + MyDialogueEventTriggers.Num() + MyAllyTriggers.Num();
			break;
		case ETriggerMode::OR:
			Threshold = 1;
			break;
		case ETriggerMode::COUNT:
			break;
		// if it's set to ANDNOT it should only trigger if none of the input triggers have been triggered
		case ETriggerMode::ANDNOT:
			Threshold = 0;
			break;
		// only care if no dialogues have been triggered but can have other inputs
		case ETriggerMode::NOTDIALOGUE:
			Threshold = 0;
			break;
		case ETriggerMode::NOTRELAY:
			Threshold = 1;
	}
	for (auto e : MyEnemyTriggers)
	{
		ADuoQEnemyCharacter* enemy = e.Get();
		if (enemy != nullptr)
		{
			if (UHealthComponent* Health = enemy->FindComponentByClass<UHealthComponent>())
			{
				Health->OnDeathWithSource.AddDynamic(this, &ATriggerTarget::EnemyKilled);
			}
		}
	}

	for (auto a : MyAllyTriggers)
	{
		if (ADuoQCharacter* Ally = a.Get())
		{
			if (UHealthComponent* Health = Ally->FindComponentByClass<UHealthComponent>())
			{
				Health->OnDeathWithSource.AddDynamic(this, &ATriggerTarget::EnemyKilled);
			}
		}
	}

	if (!MyDialogueEventTriggers.IsEmpty())
	{
		if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
		{
			DialogueSubsystem->OnEventRaised.AddDynamic(this, &ATriggerTarget::EventTriggered);
			DialogueSubsystem->OnEventLowered.AddDynamic(this, &ATriggerTarget::EventUnTriggered);
		}
	}
}

void ATriggerTarget::Trigger()
{
	Trigger(GetActorLocation());
}

void ATriggerTarget::Trigger(FVector source)
{
	if (bHasEverTriggered && bIsSingleTrigger)
		return;
	CurrentCount++;

	// check if we want no dialgoue
	if (MyTriggerMode == ETriggerMode::NOTDIALOGUE)
	{
		// negate the effect if no dialogue has been triggered
		if (DialogueCount == 0)
		{
			CurrentCount--;
		}
	}

	if (CurrentCount == Threshold)
	{
		if (MyTriggerMode == ETriggerMode::NOTRELAY)
		{
			DeactivateTrigger();
			//OnTrigger(source);
			//OnTriggered.Broadcast();
			bHasEverTriggered = true;
			return;
		}
		UE_LOG(LogTemp, Display, TEXT("LAST KILLED at %s"), *source.ToString());
		// check if we want this trigger to be delayed
		if (DelayTime != 0.0f)
		{
			MyTimerDelegate.BindUObject(this, &ATriggerTarget::OnTrigger, source);
			GetWorldTimerManager().SetTimer(MyTimer, MyTimerDelegate, DelayTime, false);
		}
		else
		{
			OnTrigger(source);
		}
		OnTriggered.Broadcast();
		bHasEverTriggered = true;
	}
}

void ATriggerTarget::OnUnTrigger_Implementation() {}

void ATriggerTarget::HandleOnTriggerActivated()
{
	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		// set the
		FAtom Atom;
		Atom.name = FString(TEXT("ShouldChooseArea"));
		Atom.value = true;

		SGC->SetDesiredWorldAtomPriority(Atom, 1);
	}

	if (AStarlightCharacter* Star = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld()))
	{
		Star->NumAreaBeat += 1;
	}
}

void ATriggerTarget::EventTriggered(FString EventName)
{
	if (MyDialogueEventTriggers.Contains(EventName) && !FinishedDialogueEventTriggers.Contains(EventName))
	{
		DialogueCount++;
		FinishedDialogueEventTriggers.Add(EventName);
		Trigger();
	}
}

void ATriggerTarget::EventUnTriggered(FString EventName)
{
	if (MyDialogueEventTriggers.Contains(EventName) && FinishedDialogueEventTriggers.Contains(EventName))
	{
		DialogueCount--;
		FinishedDialogueEventTriggers.Remove(EventName);
		UnTrigger();
	}
}

void ATriggerTarget::OnTrigger_Implementation(FVector source)
{
	// remember any logic here could very likely be overrided somewhere else
	DialogueTriggerComponent->TriggerDialogue();
	if (bIsRelay)
		ActivateTrigger(source);
}

void ATriggerTarget::UnTrigger()
{
	CurrentCount--;
	if (bIsOneWay)
		return;
	if (CurrentCount < Threshold || bIsSingleCountTrigger)
	{
		if(CurrentCount + 1 == Threshold)
		{
			if (bIsInverseRelay)
				DeactivateTrigger();
			OnUnTrigger();
		}
	}
}