// Fill out your copyright notice in the Description page of Project Settings.

#include "KillGruntEnemyPromptAction.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "Characters/DynamicGOAPComponent.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/HealthComponent.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"

void UKillGruntEnemyPromptAction::WaitForPlayerToBreakShield()
{

	// should say something like "hey can you break the shield for me?"
}

void UKillGruntEnemyPromptAction::HandleOnShieldBroken()
{
	// check if the target enemy is a grunt enemy and unshielded by the player
	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		if (AActor* TargetEnemy = SGC->GetTargetEnemy())
		{
			if (AGruntShotgunEnemy* SEnemy = Cast<AGruntShotgunEnemy>(TargetEnemy))
			{
				if (UHealthComponent* Health = SEnemy->GetComponentByClass<UHealthComponent>())
				{
					// this means the target's shield is not broken
					if (Health->bHasShield && Health->bShielded)
					{
						// should decrease SAM Meter here and say something like "this is not the enemy I chose"
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Hey that's not the enemy I targeted!"));
						}
					}
				}
			}
		}
	}
}

void UKillGruntEnemyPromptAction::TargetSelection()
{
	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		TArray<AActor*> ShotgunEnemies = SGC->GruntEnemiesInsight;
		if (ShotgunEnemies.Num() == 1)
		{
			TargetActor = ShotgunEnemies[0];
			target = ShotgunEnemies[0];
			SGC->SetTargetEnemy(ShotgunEnemies[0]);
		}
		else if (ShotgunEnemies.Num() > 1) // there are more than 1 shotgun enemies, choose the closest one from starlight
		{
			float	ShortestDistance = 1000000000.0f;
			FVector StarlightLocation = SGC->GetPawn()->GetActorLocation();

			for (AActor* ShotgunEnemy : ShotgunEnemies)
			{
				if (ShotgunEnemy != nullptr)
				{
					float Dist = FVector::Dist(ShotgunEnemy->GetActorLocation(), StarlightLocation);
					if (Dist < ShortestDistance)
					{
						ShortestDistance = Dist;
						TargetActor = ShotgunEnemies[0];
						target = ShotgunEnemy;
						SGC->SetTargetEnemy(ShotgunEnemy);
					}
				}
			}
		}
		else
		{
			TargetActor = nullptr;
			target = nullptr;
			SGC->SetTargetEnemy(nullptr);
		}

		// set the priority of the target enemy to highest
		if (SGC->GetTargetEnemy() != nullptr)
		{
			if (TObjectPtr<AGruntShotgunEnemy> CurrentTarget = Cast<AGruntShotgunEnemy>(SGC->GetTargetEnemy()))
			{
				// try to get the enemy's dynamic goap component
				if (TObjectPtr<UDynamicGOAPComponent> DGOAP = CurrentTarget->GetComponentByClass<UDynamicGOAPComponent>())
				{
					FAtom Atom;
					Atom.name = DGOAP->GetSpecificAtomName(FString(TEXT("Kill")));
					Atom.value = true;

					// set the priority to top priority
					SGC->desiredWorld[Atom] = 2;
				}
			}
		}
	}
}

void UKillGruntEnemyPromptAction::SetPertinentGoalPriority(int NewPrio)
{
	if ( TargetActor != nullptr)
	{
		AActor* Target = TargetActor;
		if (UDynamicGOAPComponent* DGOAP = Target->GetComponentByClass<UDynamicGOAPComponent>())
		{
			FAtom Atom;
			Atom.name = DGOAP->GetSpecificAtomName(FString(TEXT("Kill")));
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
