// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StunShield.generated.h"


class USphereComponent;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API AStunShield : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AStunShield();

	/* Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shield")
	USphereComponent* CollisionComp;
	
	// Mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shield")
	UStaticMeshComponent* MeshComp;

	// Disable timer
	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	float RechargeTime = 5.0f;

	// if it got hit with electric damage, disable
	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	bool bIsDisabled = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// make shield reappear after recharge time
	void EnableShield();

	// timer for disabling
	FTimerHandle DisableTimerHandle;


public:	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Delegate for overlapping
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

		
};
