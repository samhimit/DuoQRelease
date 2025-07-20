// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunPickup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGunPickUp);

UCLASS()
class DUOQ_API AGunPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunPickup();
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bIsForPlayer = true;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> PickupSound;

protected:

public:
	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintAssignable)
	FOnGunPickUp OnGunPickUp;

};
