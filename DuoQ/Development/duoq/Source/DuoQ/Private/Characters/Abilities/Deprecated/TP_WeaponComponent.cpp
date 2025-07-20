// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/Abilities/Deprecated/TP_WeaponComponent.h"
#include "Characters/DuoQCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/DamageEvents.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Player/DuoQPlayerCharacter.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if(BulletsCount <= 0)
	{
		Reload();
		return;
	}
	BulletsCount--;

	if(ADuoQPlayerCharacter* DuoQCharacter = Cast<ADuoQPlayerCharacter>(GetOwner()))
	{
		//DuoQCharacter->UpdateUI();
	}

	if(GetOwner()->GetWorldTimerManager().IsTimerActive(FireHandle))
		return;

	GetOwner()->GetWorldTimerManager().SetTimer(FireHandle,FireRate/60.0f,false);

	if(BulletsCount == 0)
		Reload();

	// Try and fire a projectile
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FRotator SpawnRotation;
		APawn* Pawn = Cast<APawn>(GetOwner());
		SpawnRotation = Pawn->GetController()->GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
		FVector EndLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(FVector(10000.0f,0.0f,0.0f));

		//instead of using character rotation as forward, we want the forward to be from the actor to its target
		//starlight specific code	
		if (AActor* owner = GetOwner())
		{
			if (AStarlightCharacter* Starlight = Cast<AStarlightCharacter>(owner))
			{
				AActor* LookAt = Starlight->CurrentTargetEnemy;
				// we want to check if the target enemy is still valid
				if (LookAt != nullptr)
				{
					// froward from starlight to enemy
					FVector StoE = LookAt->GetActorLocation() - Starlight->GetActorLocation();
					StoE.Normalize();
					EndLocation = GetOwner()->GetActorLocation() + StoE * 10000.0f;
				}
			}
		}

		
		FHitResult HitResult(ForceInit);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());
		CollisionParams.bTraceComplex = true;
		bool DidHitObject = World->LineTraceSingleByChannel(HitResult,SpawnLocation,EndLocation,ECC_WorldDynamic,CollisionParams);
		DrawDebugLine(GetWorld(), SpawnLocation, EndLocation, FColor::Green, false, 0.1f, false, 4.f);
		if(DidHitObject)
		{
			//HitResult.GetActor()->TakeDamage(Damage,FDamageEvent(),Pawn->GetController(),GetOwner());
			AActor* CActor = HitResult.GetActor(); // ...actor to damage
			FVector ShotDirection = HitResult.ImpactNormal; // ...damage direction
			TSubclassOf <UDamageType> DamageType; // ...use your own Damage Type

			// Apply damage
			FPointDamageEvent PointDamageEvent(Damage, HitResult, ShotDirection, DamageType);
			if(CActor)
			{
				CActor->TakeDamage(Damage, PointDamageEvent, GetOwner()->GetInstigatorController(), GetOwner());
			}
			
		}
		if(FireEffect != nullptr)
		{
			UGameplayStatics::SpawnEmitterAttached(FireEffect, this,TEXT("Muzzle"),FVector(),FRotator(),EAttachLocation::KeepRelativeOffset, true);
			//UGameplayStatics::SpawnEmitterAttached(FireEffect, this,"Muzzle",SpawnLocation + SpawnRotation.RotateVector(FVector(0.0f,100.0f,-20.0f)), FRotator::ZeroRotator,EAttachLocation::KeepRelativeOffset, true);
		}
		// Try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetOwner()->GetActorLocation());
		}
	}
	
	
	// Try and play a firing animation if specified
	// if (FireAnimation != nullptr)
	// {
	// 	// Get the animation object for the arms mesh
	// 	UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
	// 	if (AnimInstance != nullptr)
	// 	{
	// 		AnimInstance->Montage_Play(FireAnimation, 1.f);
	// 	}
	// }
}

void UTP_WeaponComponent::Reload()
{
	if(isReloading)
		return;
	isReloading = true;
	UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetOwner()->GetActorLocation());
	/*if (ADuoQPlayerCharacter* DuoQCharacter = Cast<ADuoQPlayerCharacter>(GetOwner()))
	{
		DuoQCharacter->ReloadAnimation();
	}*/
	FTimerHandle ReloadHandle;
	GetOwner()->GetWorldTimerManager().SetTimer(ReloadHandle, this, &UTP_WeaponComponent::RefillMagazine, ReloadTime, false);
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	BulletsCount = MagazineSize;
}

void UTP_WeaponComponent::RefillMagazine()
{
	BulletsCount = MagazineSize;
	isReloading = false;
	if(ADuoQPlayerCharacter* DuoQCharacter = Cast<ADuoQPlayerCharacter>(GetOwner()))
	{
		//DuoQCharacter->UpdateUI();
	}
}

void UTP_WeaponComponent::AttachWeapon(ADuoQPlayerCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}