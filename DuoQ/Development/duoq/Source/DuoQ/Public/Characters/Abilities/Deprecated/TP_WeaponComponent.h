// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class ADuoQCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ADuoQProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	UParticleSystem* FireEffect;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	int MagazineSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	int BulletsCount;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(class ADuoQPlayerCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Reload();

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void BeginPlay() override;

	void RefillMagazine();

	FTimerHandle FireHandle;

	

private:
	/** The Character holding this weapon*/
	class ADuoQPlayerCharacter* Character;
	bool isReloading = false;
};
