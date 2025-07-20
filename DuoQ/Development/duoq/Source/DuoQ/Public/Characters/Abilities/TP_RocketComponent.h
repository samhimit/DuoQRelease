// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_RocketComponent.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadFinish);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerUp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagazineEmpty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletFired, TSubclassOf<class ADuoQBullet>, ProjectileClass);


UENUM(BlueprintType)
enum ShootingState
{
	Idle,
	Shooting,
	Charging,
	Cooldown,
	Reloading
};
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UTP_RocketComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ADuoQBullet> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	/** Constructor of the RocketComponent class */
	UTP_RocketComponent();

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	ADuoQBullet* Fire();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	AActor* FireProjectile(class TSubclassOf<AActor> projectile, USoundBase* audio, float coneAngle = 0.0f);

	ADuoQBullet* FireProjectile(class TSubclassOf<ADuoQBullet> projectile, float coneAngle = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Tick")
	void AimAssistTick(UClass* enemyClass, class ADuoQPlayerCharacter* player, float stickyReduction);

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
};
