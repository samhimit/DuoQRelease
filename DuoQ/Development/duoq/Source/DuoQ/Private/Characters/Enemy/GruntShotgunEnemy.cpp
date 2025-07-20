// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Enemy/GruntShotgunEnemy.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/Support/WinAdapter.h>

#include "Characters/Abilities/DuoQBullet.h"
#include "Characters/Abilities/Deprecated/DuoQProjectile.h"
#include "Characters/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystems/Dialogue/StarlightResponseSubsystem.h"
#include "GameSystems/DuoQGameInstance.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "Characters/Enemy/EnemyShotgunComponent.h"
#include "Characters/DynamicGOAPComponent.h"
#include "GameSystems/SAMSubsystem/StarlightAffinityMeterSubsystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameSystems/Audio/AudioSubsystem.h"

AGruntShotgunEnemy::AGruntShotgunEnemy()
{
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComp->SetupAttachment(RootComponent);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	// target is just player for now
	ShotgunComponent = CreateDefaultSubobject<UEnemyShotgunComponent>(TEXT("ShotgunComponent"));
	ShotgunComponent->SetupAttachment(RootComponent);
}

void AGruntShotgunEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// if(HealthComp->CurrentHealth <= 0.0f)
	// {
	// 	HandleDeath();
	// }
}

void AGruntShotgunEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGruntShotgunEnemy::HandleOnShieldBreak()
{
	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		// we want to check if this grunt has ever been registered first before tweaking the kill goal
		if (UDynamicGOAPComponent* DG = GetComponentByClass<UDynamicGOAPComponent>())
		{
			// we want to check if this grunt has ever been registered first before tweaking the kill goal
			if (!DG->bHasBeenSeen)
			{
				if (bShouldRegWhenHit)
				{
					if (UHealthComponent* Health = this->GetComponentByClass<UHealthComponent>())
					{
						SGC->HandleOnPerceive(Health, DG, true, false);
						DG->bHasBeenSeen = true;
						//GetGameInstance()->GetSubsystem<UAudioSubsystem>()->AddEnemy(this);
					}
				}
			}

			FAtom Atom;
			Atom.name = DG->GetSpecificAtomName(FString(TEXT("Kill")));
			Atom.value = true;

			if (SGC->desiredWorld.Find(Atom) != nullptr)
			{
				// try to find this atom in DesiredWorld and set priority to 1
				SGC->desiredWorld[Atom] = 2;
			}

			DG->UpdateKeyedAtomStatus(FString(TEXT("IsDamageable")), true);
			//DG->UpdateKeyedAtomStatus(FString(TEXT("InSight")), true);

			// Starlight should replan with the new priorities
			SGC->TerminateCurrentAction();
			if (UPriorityGOAPAction* Action = Cast<UPriorityGOAPAction>(SGC->GetCurrentAction()))
			{
				Action->SetFinishExecute(true);
			}
		}
	}

	// check if the target enemy is a grunt enemy and unshielded by the player
	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		if (AActor* TargetEnemy = SGC->GetTargetEnemy())
		{
			if (AGruntShotgunEnemy* SEnemy = Cast<AGruntShotgunEnemy>(TargetEnemy))
			{
				if (UHealthComponent* Health = SEnemy->GetComponentByClass<UHealthComponent>())
				{
					// this means the target's shield is not broken
					if (Health->bHasShield && Health->bShielded)
					{
						// should decrease SAM Meter here and say something like "this is not the enemy I chose"
						if (GEngine)
						{
							// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Hey that's not the enemy I targeted!"));

							// decrease SAM and do call out
							if (UDuoQGameInstance* DGI = Cast<UDuoQGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
							{
								if (UStarlightAffinityMeterSubsystem* SAM = DGI->GetSubsystem<UStarlightAffinityMeterSubsystem>())
								{
									// SAM->UpdateSAMScore(-1);
								}

								if (UStarlightResponseSubsystem* SRS = DGI->GetSubsystem<UStarlightResponseSubsystem>())
								{
									// SRS->HandleCallout(WrongEnemyWave, FString(TEXT("Hey that's not the enemy I targeted!")));
								}
							}
						}
					}
				}
			}
		}
	}
}

void AGruntShotgunEnemy::HandleOnShieldRegenerate()
{
	if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(GetWorld()))
	{
		if (UDynamicGOAPComponent* DG = GetComponentByClass<UDynamicGOAPComponent>())
		{
			FAtom Atom;
			Atom.name = DG->GetSpecificAtomName(FString(TEXT("Kill")));
			Atom.value = true;

			if (SGC->desiredWorld.Find(Atom) != nullptr)
			{
				// try to find this atom in DesiredWorld and set priority to 1
				SGC->desiredWorld[Atom] = 4;
			}

			// Starlight should replan with the new priorities
			SGC->TerminateCurrentAction();
			if (UPriorityGOAPAction* Action = Cast<UPriorityGOAPAction>(SGC->GetCurrentAction()))
			{
				Action->SetFinishExecute(true);
			}
		}
	}
}

void AGruntShotgunEnemy::SetShooting(bool IsShooting)
{
	bShooting = IsShooting;
	if (bShooting)
	{
		PlayAnimMontage(ShootMontage);
	}
}

void AGruntShotgunEnemy::SetState(TEnumAsByte<EGruntShotgunState> NewState)
{
	OnCombatStateChange.Broadcast(NewState);
	CurrentState = NewState;
}

void AGruntShotgunEnemy::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnShieldBreak.AddDynamic(this, &AGruntShotgunEnemy::HandleOnShieldBreak);
	HealthComp->OnShieldRecharge.AddDynamic(this, &AGruntShotgunEnemy::HandleOnShieldRegenerate);

	// ShotgunComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Gun"));
	// FRotator GunRotation = ShotgunComponent->GetComponentRotation();
	// GunRotation.Yaw += 180.0f;
	// GunRotation.Pitch += 30.0f;
	// ShotgunComponent->AddLocalRotation(GunRotation);

	// Shotgun = GetWorld()->SpawnActor<AActor>(ShotgunClass, FVector::ZeroVector, GetActorRotation());
	//
	// if (ShotgunClass)
	// {
	// 	if (Shotgun)
	// 	{
	// 		if (const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(TEXT("Gun")))
	// 		{
	// 			Socket->AttachActor(Shotgun, GetMesh());
	// 		}
	// 	}
	// }
}

void AGruntShotgunEnemy::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	// if (OtherActor && (Cast<ADuoQBullet>(OtherActor) || Cast<ADuoQProjectile>(OtherActor)))
	// {
	// 	HealthComp->HandleDamage(DamageAmount);
	// 	UpdateHealthUI(HealthComp->CurrentHealth, HealthComp->MaxHealth);
	// 	if(HealthComp->CurrentHealth <= 0.0f)
	// 	{
	// 		HandleDeath();
	// 	}
	// 	HealthComp->GetCurrentHealth();
	// 	UE_LOG(LogTemp, Log, TEXT("Current Health: %f / %f"), HealthComp->GetCurrentHealth(), HealthComp->MaxHealth);
	// }
}
