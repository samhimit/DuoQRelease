// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/Deprecated/WallComponent.h"

// Sets default values for this component's properties
UWallComponent::UWallComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWallComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UWallComponent::SpawnWall()
{
	// Get the owner of this component
	AActor* Owner = GetOwner();

	// Check if the owner is valid
	if (Owner)
	{
		// Cast the owner to APawn
		APawn* OwnerPawn = Cast<APawn>(Owner);

		// Check if the cast was successful
		if (OwnerPawn)
		{
			FVector ActorLocation = Owner->GetActorLocation();
			FVector Offset = FVector(1000.0f, 0.0f, 0.0f);
			FRotator ControlRotation = OwnerPawn->GetControlRotation();

			

			FVector Start = ActorLocation;
			FVector End = Start + ControlRotation.RotateVector(Offset);

			FHitResult			  HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(Owner);
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Owner is not a Pawn."));
		}
	}
}


// Called every frame
void UWallComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

