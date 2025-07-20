// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DuoQBullet.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;
class ULinecastProjectile;

UCLASS(config = Game)
class DUOQ_API ADuoQBullet : public AActor
{
	GENERATED_BODY()
public:
	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Linecast component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Projectile)
	ULinecastProjectile* LinecastComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Projectile)
	UStaticMeshComponent* SphereVisual;

	/** Particle effects */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Effects")
	UNiagaraSystem* RegularDamageParticleSystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Effects")
	UNiagaraSystem* ElectricDamageParticleSystem;

	//allows certain effects to finish playing before destroying the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bLetItselfHandleDestroy;
	
	//allows certain effects to finish playing before destroying the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float DelayDestroyLifetime;

protected:
	bool bProjectilePendingDestroy;
	float DelayDestroyTimer;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void LifeSpanExpired() override;

	ADuoQBullet();

	UPROPERTY(EditAnywhere)
	float PenetrationPushOut;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	void InitParameter(int damage, AController* mInstigator, AActor* mSource, TSubclassOf<UDamageType> mDamageType);
	void BeginPlay() override;

	/** Returns damage type **/
	TSubclassOf<UDamageType> GetDamageType() const { return mDamageType; }
	
	/** Returns damage amount **/
	int			 GetDamageAmount() const { return mDamage; }
	AController* mInstigator;
	AActor*		 mSource;
	
private:
	int mDamage;
	const float	CRITICAL_MULTIPLIER = 1.5f;
	TSubclassOf<UDamageType> mDamageType;
};