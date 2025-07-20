// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/BTTask_FindPlayerLocation.h"

#include "NavigationSystem.h"
#include "Characters/DuoQCharacter.h"
#include "Kismet/GameplayStatics.h"


class ADuoQCharacter;

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation()
{
	NodeName = FString("Find Player Location");
}


EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ADuoQCharacter* const Player = Cast<ADuoQCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		FVector const PlayerLoc = Player->GetActorLocation();
		if(SearchRandom)
		{
			FNavLocation Loc;

			//get nav system for random location near player
			if (auto* const nav = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				if (nav->GetRandomPointInNavigableRadius(PlayerLoc, SearchRadius, Loc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerLoc);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	}

	
	return EBTNodeResult::Failed;
	
}

