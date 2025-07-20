// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Partner/PriorityGOAPAction.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "TimerManager.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/DynamicGOAPComponent.h"
#include "Characters/DuoQCharacter.h"

UPriorityGOAPAction::UPriorityGOAPAction()
	: UGOAPAction()
{
}

UWorld* UPriorityGOAPAction::GetWorld() const
{
	if (mWorld != nullptr)
	{
		return mWorld;
	}
	return nullptr;
}

float UPriorityGOAPAction::getCost()
{
	return cost;
}

void UPriorityGOAPAction::SetFinishExecute(bool NewValue)
{
	if (AStarlightCharacter* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld()))
	{
		if (AController* Control = Starlight->GetController())
		{
			if (AStarlightGOAPController* SC = Cast<AStarlightGOAPController>(Control))
			{
				UBlackboardComponent* Blackboard = SC->GetBlackboardComponent();
				if (Blackboard)
				{
					Blackboard->SetValueAsBool(FName("ShouldFinishExecute"), NewValue);
				}
			}
		}
	}
	// disable all timers
	if (GetWorld() != nullptr)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
}

void UPriorityGOAPAction::SetKeyedGoalPriority(int NewPrio, FString ActionName)
{
	if (AActor* Target = getTarget())
	{
		if (UDynamicGOAPComponent* DGOAP = Target->GetComponentByClass<UDynamicGOAPComponent>())
		{
			FAtom Atom;
			Atom.name = DGOAP->GetSpecificAtomName(ActionName);
			Atom.value = true;

			if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
			{
				if (SGC->desiredWorld.Contains(Atom))
				{
					SGC->desiredWorld[Atom] = NewPrio;
				}
			}
		}
	}
}

void UPriorityGOAPAction::SetTargetEnemy(ADuoQCharacter* Target)
{
	TargetActor = Target;
}

void UPriorityGOAPAction::UpdateKeyedAtomStatus(FString AtomName, bool NewState)
{
	if (AActor* Target = TargetActor)
	{
		if (UDynamicGOAPComponent* DGOAP = Target->GetComponentByClass<UDynamicGOAPComponent>())
		{
			DGOAP->UpdateKeyedAtomStatus(AtomName, NewState);
		}
	}
}

void UPriorityGOAPAction::ResetWarnTimer()
{
	NumTimeWarned += 1;
	bShouldSetTimer = true;
	bShouldWarn = true;
}

AStarlightCharacter* UPriorityGOAPAction::GetStarlightCharacter()
{
	return UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(mWorld);
}

ADuoQPlayerCharacter* UPriorityGOAPAction::GetPlayer()
{
	return UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(mWorld);
}
