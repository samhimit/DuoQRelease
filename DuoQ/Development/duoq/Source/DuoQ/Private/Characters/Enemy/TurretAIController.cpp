// Copyright 2024


#include "Characters/Enemy/TurretAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Characters/DuoQCharacter.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "Characters/Enemy/TurretEnemy.h"
#include "Characters/Enemy/TurretRotationController.h"
#include "GameSystems/Audio/AudioSubsystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

void ATurretAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

void ATurretAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
	
	if (GetOwner())
	{
		Turret = Cast<ATurretEnemy>(GetOwner());
	}
}

void ATurretAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Turret = Cast<ATurretEnemy>(InPawn);
}

AActor* ATurretAIController::FindClosestToLOSTarget()
{
	TArray<AActor*>& OutActors = CurrentActor;
	TArray<AActor*> InSightActors;
	if(const UAIPerceptionComponent* Perception = GetAIPerceptionComponent())
	{
		Perception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), OutActors);
	}
	for (auto Actor : OutActors)
	{
		if(ADuoQCharacter* Char = Cast<ADuoQCharacter>(Actor))
		{
			InSightActors.AddUnique(Char);
			if (!bSpotted)
			{
				bSpotted = true;
				//GetGameInstance()->GetSubsystem<UAudioSubsystem>()->AddEnemy(Turret);
			}
		}
	}
	
	return Turret->GetComponentByClass<UTurretRotationController>()->FindClosestActor(InSightActors);
}
