// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Partner/FireRocketGOAPAction.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "Engine/World.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Abilities/DuoQStarlightWeapon.h"

bool UFireRocketGOAPAction::CheckPlayerStatus()
{
	if (AStarlightCharacter* Star = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld()))
	{
		// check if player is behind
		if (!Star->IsPlayerInFront(70.0f))
		{
			bPlayerInPosition = false;
			if (bShouldSetTimer)
			{
				bShouldSetTimer = false;
				GetWorld()->GetTimerManager().SetTimer(WarnHandle, this, &UFireRocketGOAPAction::ResetWarningTimer, WarnWaitTime, false);
			}
			return false;
		}
		else
		{
			// starlight should strafe to the player
			if (Star->StrafeWhileMouseDead(UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(GetWorld())))
			{
				Star->bHasSetDirection = false;
				SetFinishExecute(true);
				// reset the atom
				AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld());
				FAtom					  Atom;
				Atom.name = FString(TEXT("GivePlayerRocket"));
				Atom.value = true;
				SGC->ResetTargetAtomToFalse(Atom);

				return true;
			}
			bPlayerInPosition = true;
		}
	}
	return false;
}

void UFireRocketGOAPAction::ResetWarningTimer()
{
	bShouldSetTimer = true;
	bShouldWarn = true;
}
