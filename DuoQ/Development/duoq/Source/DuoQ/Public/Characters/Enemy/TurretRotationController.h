// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TurretRotationController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRotationFinished, bool, bSuccessful);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UTurretRotationController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTurretRotationController();

	void SetYawSpeed(float NewSpeed) { YawSpeed = NewSpeed; }

	void SetPitchSpeed(float NewSpeed) { PitchSpeed = NewSpeed; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* CurrentTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector LocationCurrentTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bActivelyRotating = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRotatingTowardsObject = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsInConstantRotation;

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnRotationFinished OnRotationFinished;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Turret;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Mantlet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float YawSpeed = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float PitchSpeed = 30.0f;

	UPROPERTY(EditDefaultsOnly)
	float Tolerance = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VerticalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)

	FRotator TargetRotation = FRotator();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetupTurretParts(USceneComponent* InTurret, USceneComponent* InMantlet);

	UFUNCTION(BlueprintCallable)
	void RotateTowardsTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void RotateTowardsTargetLocation(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable)
	void ToggleConstantRotation(bool bConstantRotation);

	UFUNCTION(BlueprintCallable)
	float CalculateDesiredYawToTarget(FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	float CalculateDesiredPitchToTarget(FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	bool ReachedTarget();
	
	UFUNCTION(BlueprintCallable)
	AActor* FindClosestActor(TArray<AActor*> Actors);

	UFUNCTION(BlueprintCallable)
	FVector GetLocationOfTarget();

		
};
