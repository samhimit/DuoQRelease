// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElectroMagneticFieldComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API AElectroMagneticFieldComponent : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AElectroMagneticFieldComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* AudioComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ActivateSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* DeactivateSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* DestroySound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* AbsorbSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ReleaseSound;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* SphereVisual;
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	FComponentReference MagComp;
	FComponentReference ProjComp;
	class AMagnetizableRocket* Rocket;
	bool				InReboundingState = false;
	bool				IsActiveField = false;
	UPROPERTY(BlueprintReadWrite)
	float DistanceToCatch = 250.0f;

public:
	void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void Activate();
	UFUNCTION(BlueprintCallable)
	void		 Deactivate();
	void		 StoreProjectile(class AMagnetizableRocket * projectile);
	void		 ReleaseProjectile();
		
};
