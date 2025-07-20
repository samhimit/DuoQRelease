// Copyright 2024


#include "Characters/Enemy/TurretEnemy.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/Abilities/Deprecated/DuoQProjectile.h"
#include "Characters/Enemy/TurretProjectile.h"
#include "Characters/Enemy/TurretRotationController.h"

ATurretEnemy::ATurretEnemy()
{

	//this way of attaching the firepoint is temporary until we can attach it as a socket to a skeletal mesh
	//we're basically going to update the transform through blueprints lmaoooooo
	
	//create firepoint
	Firepoint = CreateDefaultSubobject<USceneComponent>(TEXT("Firepoint"));
	if(Firepoint)
	{
		Firepoint->SetupAttachment(GetRootComponent());
	}
	

	//create rotation controller
	RotationController = CreateDefaultSubobject<UTurretRotationController>(TEXT("RotationController"));
	if(RotationController)
	{
		RotationController->SetYawSpeed(100);
		RotationController->SetPitchSpeed(100);
	}

}

void ATurretEnemy::BeginPlay()
{
	Super::BeginPlay();
}


void ATurretEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}



void ATurretEnemy::SpawnProjectiles()
{
	
	if (Firepoint)
	{
		FActorSpawnParameters Params;
		Params.Instigator = this;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ADuoQProjectile* Projectile = GetWorld()->SpawnActor<ADuoQProjectile>(TurretProjectile, Firepoint->GetComponentLocation(), Firepoint->GetComponentRotation(), Params);
		Projectile->GetCollisionComp()->IgnoreActorWhenMoving(this, true);
		
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Firepoint->GetComponentLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FireEffect, Firepoint->GetComponentLocation());
		OnProjectileFired.Broadcast();
	}
}
