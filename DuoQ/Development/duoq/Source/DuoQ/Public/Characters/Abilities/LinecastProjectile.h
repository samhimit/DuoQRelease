// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LinecastProjectile.generated.h"

class ADuoQBullet;
// Delegate for hitting
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLineHit);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOQ_API ULinecastProjectile : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULinecastProjectile();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// initial position
	FVector InitialPos;

	// default for damage amount if owner may be null
	UPROPERTY(EditDefaultsOnly)
	int DamageAmount = 20;

	ADuoQBullet* BulletOwner;
	
	bool bHasHit = false;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Delegate for hitting
	UPROPERTY(BlueprintAssignable, Category = "Linecast Projectile Component")
	FOnLineHit OnLineHit;

		
};
