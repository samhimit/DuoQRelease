// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "GruntShotgunEnemy.generated.h"

class UEnemyShotgunComponent;
class AEnemyShotgun;
class UHealthComponent;
/**
 *
 */

UENUM(BlueprintType)
enum EGruntShotgunState : uint8
{
	GruntIdle = 0 UMETA(DisplayName = "Idle"),
	GruntStandby = 1 UMETA(DisplayName = "Standby"),
	GruntAttacking = 2 UMETA(DisplayName = "Attacking")
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStateChange, TEnumAsByte<EGruntShotgunState>, NewState);

UCLASS()
class DUOQ_API AGruntShotgunEnemy : public ADuoQEnemyCharacter
{
	GENERATED_BODY()

public:
	AGruntShotgunEnemy();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** handles the change in priority when an enemy shield breaks*/
	UFUNCTION()
	void HandleOnShieldBreak();

	/** handles the change in priority when an enemy shield regenerates*/
	UFUNCTION()
	void HandleOnShieldRegenerate();

	/** this bool handles whether or not GOAP register this enemy when hit by player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldRegWhenHit = true;

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* CollisionComp;

	// How much damage will be taken when hit (can change or add more damage types later)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Params")
	float DamageAmount = 50.0f;

	// How close to player before stopping to shoot
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Params")
	float DistanceToStartShooting = 200.0f;

	// Speed of rotation
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Params")
	float RotationSpeed = 10.0f;

	// Get shotgun
	AActor* GetShotgun() const { return Shotgun; }

	// get shotgun component
	UEnemyShotgunComponent* GetShotgunComponent() const { return ShotgunComponent; }

	UFUNCTION(BlueprintCallable)
	void SetShooting(bool IsShooting);

	UFUNCTION(BlueprintCallable)
	bool GetShooting() { return bShooting; }

	UFUNCTION()
	void SetState(TEnumAsByte<EGruntShotgunState> NewState);

	UPROPERTY(BlueprintAssignable)
	FOnCombatStateChange OnCombatStateChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun")
	UHealthComponent* HealthComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EGruntShotgunState> CurrentState;

	UPROPERTY(EditAnywhere, Category = "Shotgun")
	AActor* Shotgun;

	UPROPERTY(EditAnywhere, Category = "Shotgun")
	TSubclassOf<AActor> ShotgunClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shotgun")
	UEnemyShotgunComponent* ShotgunComponent;

	UPROPERTY(EditAnywhere, Category = "Shotgun")
	UAnimMontage* ShootMontage;

	// Handle collisions
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	// TEMP SOUND WAVE UPROPERTY
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> WrongEnemyWave;

	UPROPERTY(EditDefaultsOnly)
	bool bShooting;
};
