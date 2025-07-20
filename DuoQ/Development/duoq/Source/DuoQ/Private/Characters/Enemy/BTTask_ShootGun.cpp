// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Enemy/BTTask_ShootGun.h"

#include "AIController.h"
#include "Characters/Enemy/AGruntShotgunEnemy_AIController.h"
#include "Characters/Enemy/EnemyShotgunComponent.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_ShootGun::UBTTask_ShootGun()
{
	NodeName = FString("Shoot Gun");
}

EBTNodeResult::Type UBTTask_ShootGun::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAGruntShotgunEnemy_AIController* Controller = Cast<AAGruntShotgunEnemy_AIController>(OwnerComp.GetAIOwner());
	AGruntShotgunEnemy*				Enemy = Cast<AGruntShotgunEnemy>(Controller->GetPawn());
	const ADuoQCharacter*					Player = Cast<ADuoQCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UEnemyShotgunComponent*					Gun = Enemy->GetShotgunComponent();
	if (!Controller || !Enemy || !Player || !Gun)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->GetCharacterMovement()->StopMovementImmediately();

	Gun->ShootPellets();
	Enemy->SetShooting(true);
	

	return EBTNodeResult::Succeeded;
}
