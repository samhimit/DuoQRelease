// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/TargetingSystem.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "Characters/Partner/StarlightCharacter.h"

// Sets default values for this component's properties
UTargetingSystem::UTargetingSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

ADuoQCharacter* UTargetingSystem::GetTarget(ADuoQEnemyCharacter* Enemy)
{
	return DetermineTarget(Enemy);
}

void UTargetingSystem::AddEnemyShootingAt(ADuoQEnemyCharacter* Enemy, ADuoQCharacter* Target)
{
	if (Target == UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(GetWorld()))
	{
		if (!EnemiesShootingAtPlayer.Contains(Enemy))
		{
			EnemiesShootingAtPlayer.Add(Enemy);
		}
	}
	else if (Target == UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld()))
	{
		if (!EnemiesShootingAtStarlight.Contains(Enemy))
		{
			EnemiesShootingAtStarlight.Add(Enemy);
		}
	}
}

void UTargetingSystem::RemoveEnemyShootingAt(ADuoQEnemyCharacter* Enemy)
{
	if (!Enemy) return;

	if (EnemiesShootingAtPlayer.Contains(Enemy))
	{
		EnemiesShootingAtPlayer.Remove(Enemy);
	}
	
	if (EnemiesShootingAtStarlight.Contains(Enemy))
	{
		EnemiesShootingAtStarlight.Remove(Enemy);
	}
}


// Called when the game starts
void UTargetingSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UTargetingSystem::IsInLineOfSight(const ADuoQCharacter* Target) const
{
	const ADuoQEnemyCharacter* Enemy = Cast<ADuoQEnemyCharacter>(GetOwner());

	if (!Target || !Enemy) return false;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Enemy);

	return !GetWorld()->LineTraceSingleByChannel(HitResult, Enemy->GetActorLocation(), Target->GetActorLocation(), ECC_Visibility, CollisionParams);
	
}

float UTargetingSystem::DistanceToTarget(const ADuoQCharacter* Target) const
{
	const ADuoQEnemyCharacter* Enemy = Cast<ADuoQEnemyCharacter>(GetOwner());
	return Enemy ? FVector::Distance(Enemy->GetActorLocation(), Target->GetActorLocation()) : TNumericLimits<float>::Max();
}

ADuoQCharacter* UTargetingSystem::DetermineTarget(ADuoQEnemyCharacter* Enemy)
{
	CleanupTargets();
	
	ADuoQCharacter* Player = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(GetWorld());
	AStarlightCharacter* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld());

    OldTarget = CurrentTarget;

    const bool bIsPlayerInLOS = IsInLineOfSight(Player);
    const bool bIsStarlightInLOS = IsInLineOfSight(Starlight);

    // neither in LOS
    if (!bIsPlayerInLOS && !bIsStarlightInLOS)
    {
        // remove if there was a target
        if (OldTarget)
        {
            RemoveEnemyShootingAt(Enemy);
            CurrentTarget = nullptr; 
        }
        return nullptr; 
    }

    // If only player is in line of sight
    if (bIsPlayerInLOS && !bIsStarlightInLOS)
    {
        CurrentTarget = Player;
    }
    // only Starlight 
    else if (!bIsPlayerInLOS && bIsStarlightInLOS)
    {
        CurrentTarget = Starlight;
    }
	// both
    else
    {
        const float PlayerDistance = DistanceToTarget(Player);
        const float StarlightDistance = DistanceToTarget(Starlight);

        // both outside max dist
        if (PlayerDistance > MaxDistance && StarlightDistance > MaxDistance)
        {
            // remove if existing target
            if (OldTarget)
            {
                RemoveEnemyShootingAt(Enemy);
                CurrentTarget = nullptr;
            }
            return nullptr;
        }
        // If player within max dist
        if (PlayerDistance <= MaxDistance && StarlightDistance > MaxDistance)
        {
            CurrentTarget = Player;
        }
    	// starlight
        else if (StarlightDistance <= MaxDistance && PlayerDistance > MaxDistance)
        {
            CurrentTarget = Starlight;
        }
    	// both, pick closest 
        else
        {
            CurrentTarget = (PlayerDistance < StarlightDistance) ? Player : Starlight;
        }
    }

    // remove if old target different from new target
    if (OldTarget && OldTarget != CurrentTarget)
    {
        RemoveEnemyShootingAt(Enemy);
    }
	
    // add new target
    if (CurrentTarget)
    {
        AddEnemyShootingAt(Enemy, CurrentTarget);
    }

    return CurrentTarget;
}


// Called every frame
void UTargetingSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTargetingSystem::CleanupTargets()
{
	for (int32 i = EnemiesShootingAtPlayer.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(EnemiesShootingAtPlayer[i]))
		{
			RemoveEnemyShootingAt(EnemiesShootingAtPlayer[i]); 
		}
	}
	
	for (int32 i = EnemiesShootingAtStarlight.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(EnemiesShootingAtStarlight[i])) 
		{
			RemoveEnemyShootingAt(EnemiesShootingAtStarlight[i]); 
		}
	}
}

