// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Enemy//DuoQEnemyCharacter.h"

#include "GameSystems/SAMSubsystem/StarlightAffinityMeterSubsystem.h"
#include "GameSystems/Audio/AudioSubsystem.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "MovieSceneTracksComponentTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/Partner/StarlightTarget.h"
#include "Characters/HealthComponent.h"
#include "Characters/DynamicGOAPComponent.h"
#include "Characters/Abilities/DuoQBullet.h"
#include "Characters/Abilities/ElectricDamageType.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"
#include "Characters/Abilities/RegularDamageType.h"
#include "Characters/Enemy/EnemyAimPoint.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "Engine/DamageEvents.h"

ADuoQEnemyCharacter::ADuoQEnemyCharacter()
{
	AimPoint = CreateDefaultSubobject<UEnemyAimPoint>(TEXT("AimPoint"));
	AimPoint->SetupAttachment(RootComponent);
}

void ADuoQEnemyCharacter::GenerateGOAPActions()
{
	EnemyID = GetName();
	EnemyAliveStateName = EnemyID + TEXT("Alive");
	EnemyInSightStateName = EnemyID + TEXT("InSight");

	EnemyKillActionName = FName(EnemyID + TEXT("Kill"));
	EnemyMoveActionName = FName(EnemyID + TEXT("Move"));

	if (KillActionClass && MoveToActionClass)
	{
		if (AStarlightCharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this))
		{
			if (AStarlightGOAPController* StarlightGoapController = StarlightCharacter->GetController<AStarlightGOAPController>())
			{
				bHasBeenSeen = true;
				FAtom KillGoal = FAtom(EnemyAliveStateName, false);

				// Set up World States
				{
					TArray<FAtom> Atoms;
					Atoms.Emplace(EnemyAliveStateName, true);
					Atoms.Emplace(EnemyInSightStateName, false);
					StarlightGoapController->AddKeyedAtoms(KillGoal, Atoms);
					StarlightGoapController->desiredWorld.FindOrAdd(KillGoal) = KillGoalPriority;
				}

				// Kill action set up
				{
					TArray<FAtom> Effects;
					Effects.Emplace(EnemyAliveStateName, false);
					TArray<FAtom> Preconditions;
					Preconditions.Emplace(EnemyInSightStateName, true);
					StarlightGoapController->AddKeyedActionToGOAPPlanner(KillGoal, KillActionClass, EnemyKillActionName, Effects, Preconditions, this);
				}

				// Move action set up
				{
					TArray<FAtom> Effects;
					Effects.Emplace(EnemyInSightStateName, true);
					TArray<FAtom> Preconditions;
					StarlightGoapController->AddKeyedActionToGOAPPlanner(KillGoal, MoveToActionClass, EnemyMoveActionName, Effects, Preconditions, this);
				}
			}
		}
	}
}

void ADuoQEnemyCharacter::RemoveGOAPActions(AActor* Actor)
{
	if (AStarlightCharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this))
	{
		if (AStarlightGOAPController* StarlightGoapController = StarlightCharacter->GetController<AStarlightGOAPController>())
		{
			TArray<FAtom> Updates;
			Updates.Emplace(EnemyAliveStateName, false);
			StarlightGoapController->updateCurrentWorld(Updates);
		}
	}
}

AActor* ADuoQEnemyCharacter::ReturnValidTarget()
{
	AActor* ans = nullptr;
	TArray<AActor*> Childs;
	GetAllChildActors(Childs, false);

	for (AActor* Child : Childs)
	{
		if (AStarlightTarget* Target = Cast<AStarlightTarget>(Child))
		{
			ans = Target;
		}
	}

	return ans;
}

void ADuoQEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

float ADuoQEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float DamageValue = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// if (DamageEvent.DamageTypeClass == UElectricDamageType::StaticClass())
	// {
	// 	UNiagaraFunctionLibrary::SpawnSystemAttached(ElectricDamageParticleSystem, GetRootComponent(), TEXT("NAME_None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	// }
	// else
	// {
	// 	UNiagaraFunctionLibrary::SpawnSystemAttached(RegularDamageParticleSystem, GetRootComponent(), TEXT("NAME_None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	// }
	return DamageValue;
}

void ADuoQEnemyCharacter::HandleDeath()
{
	// 1. Set Simulate Physics on inherited Mesh
	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(true);
	}

	// 2. Set Hidden in Game for Health Bar
	UWidgetComponent* HealthBar = FindComponentByClass<UWidgetComponent>();
	if (HealthBar)
	{
		// Do something with HealthBar (e.g., hide it)
		HealthBar->SetHiddenInGame(true);
	}

	// 3. Set Active on inherited Capsule Component
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetActive(true);
	}

	// 4. Set Collision Enabled for inherited Capsule Component
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// 5. Set Collision Enabled for Sphere Component (assuming it's custom)
	USphereComponent* Sphere = FindComponentByClass<USphereComponent>();
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// 6. Set Blackboard Value as Bool using AIController
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		if (Blackboard)
		{
			Blackboard->SetValueAsBool(FName("isAlive"), false);
		}
	}

	// 8. Remove GOAPActions (custom function)
	// RemoveGOAPActions(this);

	/* if (UStarlightAffinityMeterSubsystem* SAMSubsystem = GetGameInstance()->GetSubsystem<UStarlightAffinityMeterSubsystem>())
	{
		SAMSubsystem->UpdateSAMScore(1);
	}*/

	// 7. Set Lifespan
	SetLifeSpan(5.0f);

	// handle Starlight visiblility setting and remove linked actions
	if (UDynamicGOAPComponent* DGOAP = GetComponentByClass<UDynamicGOAPComponent>())
	{
		DGOAP->RemoveLinkedActions(this, FString(TEXT("Kill")), true);
		//DGOAP->RemoveLinkedActions(this, FString(TEXT("Move")), true);
		//DGOAP->RemoveLinkedActions(this, FString(TEXT("TakeCover")), true);
		/* TArray<FAtom> Atoms;
		FAtom Atom;
		Atom.name = DGOAP->GetSpecificAtomName(FString(TEXT("TakeCover")));
		Atom.value = true;
		Atoms.Emplace(Atom);
		if (AStarlightGOAPController* Control = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
		{
			Control->updateCurrentWorld(Atoms);
		}*/
	}

	if (AStarlightGOAPController* Control = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		Control->RemoveActorFromVisibleActors(this);
		Control->GruntEnemiesInsight.Remove(this);
		Control->CacheVisibleActors();
		Control->ClearFocus(EAIFocusPriority::Default);
	}

	bIsDead = true;
	//GetGameInstance()->GetSubsystem<UAudioSubsystem>()->RemoveEnemy(this);

	UE_LOG(LogTemp, Warning, TEXT("Enemy character has died"));
}

FGenericTeamId ADuoQEnemyCharacter::GetGenericTeamId() const
{
	return ETeamIDs::Enemies;
}

void ADuoQEnemyCharacter::HandleOnSeenStarlightOrPlayer(bool bIsInSight)
{
	if (!bHasBeenSeen)
	{
		//GetGameInstance()->GetSubsystem<UAudioSubsystem>()->AddEnemy(this);
	}

	if (AStarlightGOAPController* Control = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		UHealthComponent*	   Health = GetComponentByClass<UHealthComponent>();
		UDynamicGOAPComponent* DGOAP = GetComponentByClass<UDynamicGOAPComponent>();
		if (Health && DGOAP)
		{
			// Control->HandleOnPerceiveEnemy(Health, DGOAP, bIsInSight, false);
			Control->HandleOnPerceive(Health, DGOAP, bIsInSight, false);
			bHasBeenSeen = true;
		}

		// also update Starlight's list of how many grunt enemies should be targeted
		if (AGruntShotgunEnemy* SE = Cast<AGruntShotgunEnemy>(this))
		{
			Control->GruntEenemyInSight += 1;
			Control->GruntEnemiesInsight.AddUnique(SE);
		}
	}
}
