// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Abilities/TP_RocketComponent.h"
#include "Characters/Abilities//DuoQBullet.h"
#include "Characters/Enemy/EnemyShotgunComponent.h"
#include "Characters/Player/DuoQPlayerCharacter.h"

#include "Kismet/GameplayStatics.h"

UTP_RocketComponent::UTP_RocketComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
	TraceChannel = TraceTypeQuery1;
}


ADuoQBullet* UTP_RocketComponent::Fire()
{
	return FireProjectile(ProjectileClass);
}
ADuoQBullet* UTP_RocketComponent::FireProjectile(TSubclassOf<ADuoQBullet> projectile, float coneAngle)
{
	return static_cast<ADuoQBullet*>(FireProjectile(projectile, FireSound, coneAngle));
}

AActor* UTP_RocketComponent::FireProjectile(TSubclassOf<AActor> projectile, USoundBase* audio, float coneAngle)
{
	
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FRotator SpawnRotation;
		APawn*	 Pawn = Cast<APawn>(GetOwner());
		
		if (Pawn)
		{
			UEnemyShotgunComponent* Weapon = Cast<UEnemyShotgunComponent>(Pawn->GetComponentByClass(UEnemyShotgunComponent::StaticClass()));
			if (Weapon)
			{
				SpawnRotation = Weapon->GetComponentRotation();
				SpawnRotation.Yaw += 90.0f;
			}
			else
			{
				SpawnRotation = Pawn->GetController()->GetControlRotation();
			}
		}
		
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		FVector SpawnLocation = GetOwner()->GetActorLocation();
		const FVector EndLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(FVector(10000.0f, 0.0f, 0.0f));

		FHitResult			  HitResult(ForceInit);
		FCollisionQueryParams CollisionParams;
		
		CollisionParams.AddIgnoredActor(GetOwner());
		CollisionParams.bTraceComplex = true;

		const ECollisionChannel Channel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);

		bool DidHitObject = World->LineTraceSingleByChannel(HitResult, SpawnLocation + SpawnRotation.RotateVector(FVector(0.0f, 0.0f, MuzzleOffset.Z)), EndLocation, Channel, CollisionParams);
		// Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		SpawnLocation = GetSocketLocation(FName("MuzzleSocket"));
		//SpawnLocation + SpawnRotation.RotateVector(MuzzleOffset);
		if (DidHitObject)
		{
			SpawnRotation = (HitResult.ImpactPoint - SpawnLocation).Rotation();
		}
		// Spawn the projectile at the muzzle
		SpawnRotation.Pitch += FMath::FRandRange(-coneAngle, coneAngle);
		SpawnRotation.Yaw += FMath::FRandRange(-coneAngle, coneAngle);
		if (audio)
		{
			UGameplayStatics::PlaySoundAtLocation(this, audio, GetOwner()->GetActorLocation());
		}
		return World->SpawnActor<AActor>(projectile, SpawnLocation, SpawnRotation, ActorSpawnParams);
	
	}
	return nullptr;
}
void UTP_RocketComponent::AimAssistTick(UClass* enemyClass, ADuoQPlayerCharacter* player, float stickyReduction)
{
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FRotator SpawnRotation;
		APawn*	 Pawn = Cast<APawn>(GetOwner());
		SpawnRotation = Pawn->GetController()->GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
		const FVector EndLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(FVector(10000.0f, 0.0f, 0.0f));

		FHitResult			  HitResult(ForceInit);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());
		bool DidHitObject = World->LineTraceSingleByChannel(HitResult, SpawnLocation, EndLocation, ECC_WorldDynamic, CollisionParams);
		DrawDebugLine(GetWorld(), SpawnLocation, EndLocation, FColor::Blue, false, 0.1f, false, 4.f);
		if (DidHitObject)
		{
			// HitResult.GetActor()->TakeDamage(Damage,FDamageEvent(),Pawn->GetController(),GetOwner());
			AActor* CActor = HitResult.GetActor(); // ...actor to damage
			player->SetSensitivity(CActor && CActor->GetClass()->IsChildOf(enemyClass) ? stickyReduction : 1.0f);
		}
	}
}