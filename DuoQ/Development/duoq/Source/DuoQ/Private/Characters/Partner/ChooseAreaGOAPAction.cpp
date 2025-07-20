// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Partner/ChooseAreaGOAPAction.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Partner/StarlightGOAPController.h"

bool UChooseAreaGOAPAction::FindNextArea()
{
	FAreaPreset Output;
	if (AStarlightCharacter* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld()))
	{
		// starlight always chooses the first area
		if (Starlight->NumAreaBeat == 0)
		{
			// randomly chooses a place
			Starlight->CurrentArea = FMath::RandRange(0, Starlight->AreasInLevel.Num() - 1);
			if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
			{
				SGC->IsLeading = true;
			}
			return true;
		}
		else // let player choose it
		{
			if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
			{
				SGC->IsLeading = false;
			}
			return false;
		}
	}
	return false;
}

void UChooseAreaGOAPAction::SetPertinentGoalPriority(int NewPrio)
{
	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		FAtom Atom;
		Atom.name = FString(TEXT("ShouldChooseArea"));
		Atom.value = true;

		SGC->SetDesiredWorldAtomPriority(Atom, NewPrio);
	}
}
