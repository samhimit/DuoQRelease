// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemyShotgunComponent.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/Abilities/RegularDamageType.h"
#include "Characters/Enemy/EnemyPellet.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"

// Sets default values
UEnemyShotgunComponent::UEnemyShotgunComponent()
{
	
}

// Called when the game starts or when spawned
void UEnemyShotgunComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UEnemyShotgunComponent::ShootPellets()
{
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetComponentLocation());

	for (int i = 0; i < NumberOfPellets; i++)
	{
		ADuoQBullet* Bullet = static_cast<ADuoQBullet*>(FireProjectile(ProjectileClass, nullptr, ShotgunSpread));
		if(!Bullet) continue;
		Bullet->InitParameter(ShotgunDamage, GetOwner()->GetInstigatorController(), GetOwner(), URegularDamageType::StaticClass());
	}
}


