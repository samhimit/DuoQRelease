// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DuoQLightBeam.generated.h"

class UCapsuleComponent;

UCLASS(config=Game)
class DUOQ_API ADuoQLightBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADuoQLightBeam();

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 3.0f;

	UPROPERTY(EditDefaultsOnly)
	float InRadius = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float HalfHeight = 10.0f;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CollisionComp;

};
