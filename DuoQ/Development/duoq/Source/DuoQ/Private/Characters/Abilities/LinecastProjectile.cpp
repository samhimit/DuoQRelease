// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Abilities/LinecastProjectile.h"

#include "Characters/DuoQCharacter.h"
#include "Characters/Abilities/DuoQBullet.h"
#include "Characters/Abilities/ElectricDamageType.h"
#include "Characters/Abilities/MagnetizableRocket.h"
#include "Characters/Abilities/RegularDamageType.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
ULinecastProjectile::ULinecastProjectile()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// RegisterComponent();

	TraceChannel = TraceTypeQuery1;
}

// Called when the game starts
void ULinecastProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetActive(true);
	if (ADuoQBullet* Bullet = Cast<ADuoQBullet>(GetOwner()))
	{
		BulletOwner = Bullet;
		InitialPos = GetOwner()->GetActorLocation();
	}
}

// Called every frame
void ULinecastProjectile::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check behind projectile for collisions
	const FVector		  CurrentPos = GetOwner()->GetActorLocation();
	FHitResult			  HitResult;
	FCollisionQueryParams CollisionParams;

	if (GetOwner() != nullptr)
	{
		CollisionParams.AddIgnoredActor(GetOwner());
	}

	if (BulletOwner != nullptr)
	{
		if (BulletOwner->mInstigator != nullptr)
		{
			if (BulletOwner->mInstigator->GetPawn() != nullptr)
			{

				CollisionParams.AddIgnoredActor(BulletOwner->mInstigator->GetPawn());
			}
		}

		if (BulletOwner->mSource != nullptr)
		{
			CollisionParams.AddIgnoredActor(BulletOwner->mSource);
		}
	}

	CollisionParams.bTraceComplex = true;

	const ECollisionChannel Channel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, InitialPos, CurrentPos, Channel, CollisionParams))
	{
		// #if WITH_EDITOR
		// DrawDebugLine(GetWorld(), InitialPos, HitResult.ImpactPoint, FColor::Red, false, 1.0f, 0, 1.0f);
		// #endif
		if (BulletOwner && !bHasHit)
		{
			// if (ADuoQCharacter* Hit = Cast<ADuoQCharacter>(HitResult.GetActor()))
			//{
			BulletOwner->OnHit(HitResult.GetComponent(), HitResult.GetActor(), HitResult.GetComponent(), FVector::ZeroVector, HitResult);
			//}
		}

		bHasHit = true;
		OnLineHit.Broadcast();
		if (!Cast<AMagnetizableRocket>(BulletOwner) && !BulletOwner->bLetItselfHandleDestroy)
		{
			GetOwner()->Destroy();
		}
	}
}
