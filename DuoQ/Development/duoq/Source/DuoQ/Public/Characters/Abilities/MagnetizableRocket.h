// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LinecastProjectile.h"
#include "Characters/Abilities/DuoQBullet.h"
#include "Components/ActorComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "MagnetizableRocket.generated.h"

UCLASS()
class DUOQ_API AMagnetizableRocket : public ADuoQBullet
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AMagnetizableRocket();
	UFUNCTION(BlueprintCallable)
	bool GetValid() { return true; }
	void SetMagnetizableSource(USceneComponent* target)
	{
		ProjectileMovement->HomingTargetComponent = target;
		ProjectileMovement->bIsHomingProjectile = true;
		LinecastComp->Deactivate();
	}
	void RemoveMagnetizableSource()
	{
		ProjectileMovement->HomingTargetComponent = nullptr;
		ProjectileMovement->bIsHomingProjectile = false;
		LinecastComp->Activate();
	}
	void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* RocketRelease;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* RocketExplosion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UAudioComponent> RocketLoop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* DamageCauser = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float ExplosionRadius = 50.0f;
	UPROPERTY(BlueprintReadWrite)
	float ExplosionDamage = 20.0f;

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
		
};
