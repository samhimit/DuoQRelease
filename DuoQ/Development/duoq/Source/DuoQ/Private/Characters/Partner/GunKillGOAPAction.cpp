// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Partner/GunKillGOAPAction.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Characters/Enemy//DuoQEnemyCharacter.h"
#include "Characters/Partner/StarlightTarget.h"
#include "Characters/DynamicGOAPComponent.h"
#include "Characters/Partner/StarlightSeeComponent.h"
#include "TimerManager.h"
#include "Characters/HealthComponent.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "AIController.h"

UGunKillGOAPAction::UGunKillGOAPAction()
{
	OnActionComplete.AddDynamic(this, &UGunKillGOAPAction::HandleOnActionComplete);
}

void UGunKillGOAPAction::BroadCastSuccess()
{
	OnActionComplete.Broadcast();
}

void UGunKillGOAPAction::HandleStopShoot(FTimerHandle Handle, FTimerHandle EnemyHandle)
{
	// disable all timers
	if (GetWorld() != nullptr)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}

	if (AStarlightCharacter* Starlight = GetStarlightCharacter())
	{
		if (AStarlightGOAPController* Controller = Cast<AStarlightGOAPController>(Starlight->GetController()))
		{
			Controller->ClearFocus(EAIFocusPriority::Default);
			Controller->ClearTargetEnemy();
		}
	}
}

void UGunKillGOAPAction::StarlightShoot()
{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &UGunKillGOAPAction::ShootTimer, Duration, false);
}

bool UGunKillGOAPAction::AimUpdate()
{
	// check if Starlight has a target
	if (TargetActor != nullptr)
	{
		AActor* Target = TargetActor;
		// set the target as Starlight's new focus
		AStarlightCharacter* Starlight = GetStarlightCharacter();
		Starlight->CurrentTargetEnemy = Target;

		AActor* TargetToAimAt = Target;
		if (ADuoQEnemyCharacter* Enemy = Cast<ADuoQEnemyCharacter>(Target))
		{
			TargetToAimAt = Enemy->ReturnValidTarget();
		}

		// try aiming at the chest
		FVector FocalPoint;
		if (TargetToAimAt != nullptr)
		{
			FocalPoint = TargetToAimAt->GetActorLocation();
		}
		else
		{
			FocalPoint = Target->GetActorLocation();
		}

		/* if (ADuoQEnemyCharacter* Enemy = Cast<ADuoQEnemyCharacter>(Target))
		{
			FocalPoint = Enemy->GetMesh()->GetSocketLocation(FName("Head"));
		}
		else
		{
			FocalPoint = Target->GetActorLocation();
		}*/

		// check if enemy is still insight
		// linetrace to check if something is in between AI and player
		ADuoQEnemyCharacter*		   Enemy = Cast<ADuoQEnemyCharacter>(Target);
		FHitResult					   HitResult;
		TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Camera;
		FCollisionQueryParams		   QueryParams;
		QueryParams.AddIgnoredActor(GetStarlightCharacter());

		// FVector GripPoint = Starlight->GetMesh()->GetSocketLocation(FName("GripPoint"));
		FVector GripPoint = Starlight->GetMesh()->GetBoneLocation(FName("hand_r"));
		FVector TargetHead;
		if (Enemy)
		{
			TargetHead = Enemy->GetMesh()->GetBoneLocation(FName("crest"));
		}

		AAIController* Controller = Cast<AAIController>(Starlight->GetController());

		// TargetHead = FocalPoint;

		if (mWorld->LineTraceSingleByChannel(HitResult, Starlight->GetActorLocation(), TargetHead, TraceChannelProperty, QueryParams))
		{
			// see if the hit actor is the target actor
			TObjectPtr<AActor> ResultActor = HitResult.GetActor();
			if (ResultActor != Target)
			{
				if (Enemy)
				{
					UDynamicGOAPComponent* DGOAP = Enemy->GetComponentByClass<UDynamicGOAPComponent>();
					UHealthComponent*	   Health = Enemy->GetComponentByClass<UHealthComponent>();
					if (DGOAP && Health)
					{
						if (AStarlightGOAPController* SGO = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(mWorld))
						{
							SGO->HandleOnPerceive(Health, DGOAP, false, true);
							SGO->TerminateCurrentAction();
							if (UPriorityGOAPAction* Action = Cast<UPriorityGOAPAction>(SGO->GetCurrentAction()))
							{
								Action->SetFinishExecute(true);
							}
						}
					}
				}
				if (AStarlightGOAPController* StarlightController = Cast<AStarlightGOAPController>(Controller))
				{
					StarlightController->ClearFocus(EAIFocusPriority::Default);
				}
				return false;
			}
			else
			{
				if (Enemy)
				{
					if (Controller && !Starlight->bIsMouseDead)
					{
						// Controller->SetFocus(Target);
						Controller->SetFocalPoint(TargetHead);
					}
					else if (Controller)
					{
						Controller->ClearFocus(EAIFocusPriority::Default);
					}
					UDynamicGOAPComponent* DGOAP = Enemy->GetComponentByClass<UDynamicGOAPComponent>();
					UHealthComponent*	   Health = Enemy->GetComponentByClass<UHealthComponent>();
					if (DGOAP && Health)
					{
						UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(mWorld)->HandleOnPerceive(Health, DGOAP, true, true);
					}
				}
			}
		}

		// checking preconditions to see if starlight should stopshooting or not
		if (Controller)
		{
			if (AStarlightGOAPController* StarlightController = Cast<AStarlightGOAPController>(Controller))
			{
				if (preconditions.IsEmpty())
				{
					return true;
				}
				// if the precondition holds, it means the enemy is not yet dead, so starlight should keep firing
				if (StarlightController->CurrentWorldContainsState(preconditions))
				{
					return true;
				}
				else // enemy is dead
				{
					StarlightController->ClearFocus(EAIFocusPriority::Default);
					return false;
				}
			}
		}
	}
	return true;
}

void UGunKillGOAPAction::ShootTimer()
{
}
