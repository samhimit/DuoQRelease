// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/BTTask_FindRandomLocation.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemy/AGruntShotgunEnemy_AIController.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = FString("Find Random Location");
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// get AI controller 
	if (auto* const Controller = Cast<AAGruntShotgunEnemy_AIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const Enemy = Controller->GetPawn())
		{
			FVector const Origin = Enemy->GetActorLocation();

			//get nav system 
			if (auto* const Nav = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation Loc;
				if (Nav->GetRandomPointInNavigableRadius(Origin, SearchRadius, Loc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
				}

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
