
#pragma once


#include "CoreMinimal.h"
#include "Characters/Abilities/TP_RocketComponent.h"
#include "DuoQElectricRevolver.generated.h"

UENUM(BlueprintType)
enum EPlayerGunState { ReadyToFire, WindingDown };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatePlayerGunState, TEnumAsByte<EPlayerGunState>, NewGunState);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUOQ_API UDuoQElectricRevolver : public UTP_RocketComponent
{
	GENERATED_BODY()

	UDuoQElectricRevolver();
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ADuoQBullet> ChargedProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float WindDownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TObjectPtr<class UNiagaraSystem> ElectricBulletParticles;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void TriggerDown();

	UFUNCTION(BlueprintCallable, Category = "Tick")
	void  WeaponTick(float deltaTime);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	float StateTimer = 0.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTriggerDown OnTriggerDown;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBulletFired OnBulletFired;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	bool bIsElectric = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ChargedShot;
	UPROPERTY(EditANywhere, BlueprintReadWRite, Category = "Weapon")
	bool bIsUsable = false;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TEnumAsByte<EPlayerGunState> GetState() const {return GunState;};

	UPROPERTY(BlueprintAssignable)
	FUpdatePlayerGunState OnGunStateUpdated;

protected:
	
	EPlayerGunState GunState;
	
	virtual void BeginPlay() override;

	void SpawnElectricProjectile();

	void UpdateState(EPlayerGunState NewState);
};
