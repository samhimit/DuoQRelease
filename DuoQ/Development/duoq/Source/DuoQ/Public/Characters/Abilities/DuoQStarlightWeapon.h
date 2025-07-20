
#pragma once

#include "CoreMinimal.h"
#include "Characters/Abilities/TP_RocketComponent.h"
#include "DuoQStarlightWeapon.generated.h"

class UNiagaraSystem;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUOQ_API UDuoQStarlightWeapon : public UTP_RocketComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float BloomConeAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float ReloadSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float MagSize = 6;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float ADSSpped;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void TriggerDown();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void TriggerUp();

	UFUNCTION(BlueprintCallable, Category = "Tick")
	void WeaponTick(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	float StateTimer = 0.0f;
	UPROPERTY(BlueprintReadWrite, Category = Weapon)
	bool bIsUsable = false;
	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	TEnumAsByte<ShootingState> GunState = ShootingState::Idle;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAnimMontage> ShootingAnim;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundBase> ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	FComponentReference ShootingAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	FRuntimeFloatCurve ShootingLayerGain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	FRuntimeFloatCurve ShootingPitchShift;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Effects")
	UNiagaraSystem* ShootingParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle Effects")
	UNiagaraSystem* BulletParticleSystem;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReload OnReload;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadFinish OnReloadFinish;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTriggerUp OnTriggerUp;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTriggerDown OnTriggerDown;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBulletFired OnBulletFired;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMagazineEmpty OnMagazineEmpty;

	UPROPERTY(BlueprintReadOnly)
	int CurrentMag;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleOnBulletFired(TSubclassOf<class ADuoQBullet> projectile);

	bool IsHoldingTrigger = false;

	UAudioComponent* ShootingSound;

	float ShootingTime = 0.0f;
};
