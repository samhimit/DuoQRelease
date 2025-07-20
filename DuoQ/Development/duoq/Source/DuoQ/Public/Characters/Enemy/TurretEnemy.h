// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Characters/Abilities/Deprecated/DuoQProjectile.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "TurretEnemy.generated.h"

class UWeakpointController;
class UTurretRotationController;

UENUM(BlueprintType)
enum class ETurretState : uint8
{
	Targeting UMETA(DisplayName = "Targeting"),
	Reloading UMETA(DisplayName = "Reloading")
};
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishFiring);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProjectileFired);

UCLASS(Blueprintable)
class DUOQ_API ATurretEnemy : public ADuoQEnemyCharacter
{
	GENERATED_BODY()

public:

	ATurretEnemy();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles();


protected:

	UPROPERTY(BlueprintAssignable)
	FOnFinishFiring OnFinishFiring;
	
	UPROPERTY(BlueprintAssignable)
	FOnProjectileFired OnProjectileFired;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UTurretRotationController* RotationController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	USceneComponent* Firepoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ADuoQProjectile> TurretProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* FireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* FireEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* HitSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* HitEffect;
	
};
