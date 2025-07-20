// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Enemy/BTTask_RotateGun.h"

#include "Characters/Enemy/AGruntShotgunEnemy_AIController.h"
#include "Characters/Enemy/EnemyShotgunComponent.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"

UBTTask_RotateGun::UBTTask_RotateGun()
{
	NodeName = FString("Rotate Gun");
}

EBTNodeResult::Type UBTTask_RotateGun::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAGruntShotgunEnemy_AIController* Controller = Cast<AAGruntShotgunEnemy_AIController>(OwnerComp.GetAIOwner());
	AGruntShotgunEnemy* Enemy = Cast<AGruntShotgunEnemy>(Controller->GetPawn());
	const ADuoQCharacter* Target = Cast<ADuoQCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
	UEnemyShotgunComponent* Shotgun = Enemy->GetShotgunComponent();

	if (!Controller || !Enemy || !Target || !Shotgun)
	{
		return EBTNodeResult::Failed;
	}
	FRotator ShotgunRotation = (Target->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal().Rotation();
	ShotgunRotation.Yaw -= 90.0f;
	
	Shotgun->SetWorldRotation(ShotgunRotation);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
