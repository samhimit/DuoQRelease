// Copyright 2024


#include "GameSystems/CombatManagerTrigger.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/Partner/StarlightCharacter.h"

// Sets default values
ACombatManagerTrigger::ACombatManagerTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACombatManagerTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (ADuoQPlayerCharacter* Player = Cast<ADuoQPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		Player->OnPlayerDeath.AddDynamic(this, &ACombatManagerTrigger::HandlePlayerDeath);
		Player->OnPlayerRevive.AddDynamic(this, &ACombatManagerTrigger::HandlePlayerRevive);
	}
}

// Called every frame
void ACombatManagerTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatManagerTrigger::ActivateEnemies()
{
	if (CombatManager && bCanActivate)
	{

		if (Grunts.Num() > 0)
		{
			CombatManager->AddGrunts(Grunts);
		}

		CombatManager->ActivateGrunts();
		bCanActivate = false;
	}
}

// Called when the player dies
void ACombatManagerTrigger::HandlePlayerDeath()
{
	if (CombatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player died. Setting all grunts to standby."));
		CombatManager->SetAllGruntsToStandby();
	}
}

// Called when the player revives
void ACombatManagerTrigger::HandlePlayerRevive()
{
	UE_LOG(LogTemp, Warning, TEXT("Player revived. Reactivating trigger."));
	bCanActivate = true;
}

