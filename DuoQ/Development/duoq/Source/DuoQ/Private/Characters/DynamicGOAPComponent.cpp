// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/DynamicGOAPComponent.h"
#include "GOAPAction.h"
#include "Characters/Partner/StarlightGOAPController.h"

// Sets default values for this component's properties
UDynamicGOAPComponent::UDynamicGOAPComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	if (AActor* Owner = GetOwner())
	{
		NameID = Owner->GetName();
	}
	AliveStateName = NameID + TEXT("Kill");
	InSightStateName = NameID + TEXT("InSight");

	// ...
}


void UDynamicGOAPComponent::RemoveLinkedActions(AActor* Actor, FString Name, bool Status)
{
	if (AStarlightCharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this))
	{
		if (AStarlightGOAPController* StarlightGoapController = StarlightCharacter->GetController<AStarlightGOAPController>())
		{
			// AliveStateName = NameID + TEXT("Kill");
			AliveStateName = NameID + Name;
			TArray<FAtom> Updates;
			Updates.Emplace(AliveStateName, Status);
			TArray<FAtom> CurrentWorldAtom = StarlightGoapController->getCurrentWorldStateAtoms();
			if (CurrentWorldAtom.Contains(Updates[0]))
			{
				StarlightGoapController->updateCurrentWorld(Updates);
			}
		}
	}
}






void UDynamicGOAPComponent::SetGoalAndCreateActions(FDynamicGOAPPreset Setting)
{
	if (AStarlightCharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this))
	{
		if (AStarlightGOAPController* StarlightGoapController = StarlightCharacter->GetController<AStarlightGOAPController>())
		{
			AliveStateName = NameID + TEXT("Kill");
			InSightStateName = NameID + TEXT("InSight");
			bHasBeenSeen = true;
			// set up keyed goal name as NameID + KeyedGoalName
			Setting.KeyedGoal.name = NameID + Setting.KeyedGoal.name;

			// set up keyed goal and keyed atoms
			TArray<FAtom> Atoms;
			for (FAtom Atom : Setting.AtomsToAdd)
			{
				// keyed atom name is ID of owner + specified atom name like Alive or InSight
				FString SpecifiedName = Atom.name;
				Atom.name = NameID + SpecifiedName;
				AtomNames.Emplace(Atom.name);
				Atoms.Emplace(Atom);
			}
			StarlightGoapController->AddKeyedAtoms(Setting.KeyedGoal, Atoms);
			StarlightGoapController->desiredWorld.FindOrAdd(Setting.KeyedGoal) = Setting.DefaultPriority;

			// setting up all the actions
			for (FActionMaker ActionMaker : Setting.ActionsToMake)
			{
				if (AActor* Actor = GetOwner())
				{
					// set up the name of the action, which is NameID + ActionName
					ActionMaker.NameToAdd = FName(NameID + ActionMaker.ActionName);
					// FName Action = FName(NameID + ActionMaker.ActionName);
					//  set up precondition name and effect name to be NameID + AtomName

					for (int i = 0; i < ActionMaker.Effects.Num(); i++)
					{
						ActionMaker.Effects[i].name = NameID + ActionMaker.Effects[i].name;
					}

					for (int i = 0; i < ActionMaker.Preconditions.Num(); i++)
					{
						ActionMaker.Preconditions[i].name = NameID + ActionMaker.Preconditions[i].name;
					}

					StarlightGoapController->AddKeyedActionToGOAPPlanner(Setting.KeyedGoal, ActionMaker.ActionClass,
						ActionMaker.NameToAdd, ActionMaker.Effects, ActionMaker.Preconditions, Actor);
				}
			}
		}
	}
}

void UDynamicGOAPComponent::CreateAllGoalAndActions()
{
	// call SetGoalAndCreateActions on all the Settings
	for (FDynamicGOAPPreset Setting : Settings)
	{
		SetGoalAndCreateActions(Setting);
	}
}


FString UDynamicGOAPComponent::GetSpecificAtomName(FString KeyWord)
{
	FString Output = FString();

	for (FString Atom : AtomNames)
	{
		// found the atom with the given keyword
		if (Atom.Contains(KeyWord))
		{
			Output = Atom;
			return Output;
		}
	}
	return Output;
}

void UDynamicGOAPComponent::UpdateKeyedAtomStatus(FString AtomName, bool NewState)
{
	TArray<FAtom> Atoms;
	FAtom		  Atom;
	Atom.name = GetSpecificAtomName(AtomName);
	Atom.value = NewState;
	Atoms.Emplace(Atom);

	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		SGC->updateCurrentWorld(Atoms);
	}
}

void UDynamicGOAPComponent::SetPrioWithSpecificAtom(FString AtomName, int NewPrio)
{
	FAtom Atom;
	Atom.name = GetSpecificAtomName(AtomName);
	Atom.value = true;

	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		if (SGC->desiredWorld.Contains(Atom))
		{
			SGC->desiredWorld[Atom] = NewPrio;
		}
	}
}

// Called when the game starts
void UDynamicGOAPComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UDynamicGOAPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
