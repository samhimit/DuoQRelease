// Copyright 2024


#include "Characters/Enemy/TurretRotationController.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UTurretRotationController::UTurretRotationController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTurretRotationController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTurretRotationController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bActivelyRotating)
	{
		FRotator TurretRotation = Turret->GetComponentRotation();
		TurretRotation.Pitch = 0.0f;
		TurretRotation.Roll = 0.0f;
		TargetRotation.Yaw = CalculateDesiredYawToTarget(GetLocationOfTarget());
		FRotator YawRot = FMath::RInterpConstantTo(TurretRotation, TargetRotation, DeltaTime, YawSpeed);
		YawRot.Pitch = 0.0f;
		YawRot.Roll = 0.0f;
		
		FRotator targetPitch = UKismetMathLibrary::FindLookAtRotation(Mantlet->GetComponentLocation(), CurrentTarget->GetActorLocation());
		YawRot += FRotator(0.0f, 0.0f, -targetPitch.Pitch);
		TargetRotation = YawRot;
		Turret->SetWorldRotation(FMath::RInterpConstantTo(Turret->GetComponentRotation(), YawRot, DeltaTime, PitchSpeed));

		if (ReachedTarget() && !bIsInConstantRotation)
		{
			bActivelyRotating = false;
			OnRotationFinished.Broadcast(true);
		}
	}
}

void UTurretRotationController::SetupTurretParts(USceneComponent* InTurret, USceneComponent* InMantlet)
{
	Turret = InTurret;
	Mantlet = InMantlet;
	VerticalOffset = FVector(0.0f,0.0f,1.0f) * Mantlet->GetComponentLocation().Z - InTurret->GetComponentLocation().Z;
}

void UTurretRotationController::RotateTowardsTarget(AActor* TargetActor)
{
	CurrentTarget = TargetActor;
	bActivelyRotating = true;
	bRotatingTowardsObject = true;
}

void UTurretRotationController::RotateTowardsTargetLocation(const FVector& TargetLocation)
{
	LocationCurrentTarget = TargetLocation;
	bActivelyRotating = true;
	bRotatingTowardsObject = false;
}

void UTurretRotationController::ToggleConstantRotation(bool bConstantRotation)
{
	bIsInConstantRotation = bConstantRotation;
}

float UTurretRotationController::CalculateDesiredYawToTarget(FVector TargetLocation)
{
	FVector TurretLocation = Turret->GetComponentLocation();
	TurretLocation.Z = 0.0f;
	TargetLocation.Z = 0.0f;
	FVector NewLoc = TargetLocation - TurretLocation;
	FRotator NewRot = UKismetMathLibrary::MakeRotFromZ(-NewLoc);
	return NewRot.Yaw;
	
}

float UTurretRotationController::CalculateDesiredPitchToTarget(FVector TargetLocation)
{
	FVector MantletLocation = Mantlet->GetComponentLocation();
	FVector NewLoc = UKismetMathLibrary::NegateVector(MantletLocation - TargetLocation);
	FRotator NewRot = UKismetMathLibrary::MakeRotFromX(NewLoc);
	return NewRot.Pitch;
}

bool UTurretRotationController::ReachedTarget()
{
	bool EqualYaw = FMath::IsNearlyEqual(Turret->GetComponentRotation().Yaw, TargetRotation.Yaw, Tolerance);
	bool	 EqualPitch = FMath::IsNearlyEqual(Mantlet->GetRelativeRotation().Pitch, TargetRotation.Pitch, Tolerance);
	return EqualYaw && EqualPitch;
	
}

AActor* UTurretRotationController::FindClosestActor(TArray<AActor*> Actors)
{
	float LargestDotProduct = -1.0f;
	AActor* ClosestActor = nullptr;
	for (auto Actor : Actors)
	{
		FVector TurretToActor = Actor->GetActorLocation() - Turret->GetComponentLocation();
		TurretToActor.Normalize(0.0001f);
		FVector TurretForward = Turret->GetForwardVector();
		TurretForward.Z = 0.0f;
		TurretToActor.Z = 0.0f;
		float Dot = FVector::DotProduct(TurretToActor, TurretForward);
		if (Dot >= LargestDotProduct)
		{
			LargestDotProduct = Dot;
			ClosestActor = Actor;
		}
	}
	return ClosestActor;
}

FVector UTurretRotationController::GetLocationOfTarget()
{
	if(CurrentTarget)
	{
		return bRotatingTowardsObject ? CurrentTarget->GetActorLocation() : LocationCurrentTarget;
	}
	return LocationCurrentTarget;
}

