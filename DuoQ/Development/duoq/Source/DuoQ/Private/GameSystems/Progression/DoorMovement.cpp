// Copyright 2024


#include "GameSystems/Progression/DoorMovement.h"


// Sets default values for this component's properties
UDoorMovement::UDoorMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DoorOpenSpeed = 500.0f;
	DoorGravity = 2000.0f;
	DoorStoppingAcceleration = 3000.0f;
}


// Called when the game starts
void UDoorMovement::BeginPlay()
{
	Super::BeginPlay();

	DoorState = EDoorState::Closed;
	
}

bool UDoorMovement::MoveDoorToPosition(FVector Target, float MaxDistanceDelta)
{
	FVector ToVector = Target - Door->GetComponentLocation();

	float SquaredLength = ToVector.SquaredLength();

	if(MaxDistanceDelta * MaxDistanceDelta >= SquaredLength)
	{
		Door->SetWorldLocation(Target);
		return true;
	}

	ToVector.Normalize();
	Door->SetWorldLocation(Door->GetComponentLocation() + ToVector * MaxDistanceDelta);
	return false;
}


// Called every frame
void UDoorMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(DoorState == Opening)
	{
		if(ZVelocity < 0)
		{
			ZVelocity += DoorStoppingAcceleration * DeltaTime;
		} else
		{
			ZVelocity = DoorOpenSpeed;
		}
		bool bFinishedMove = MoveDoorToPosition(DoorOpenedPosition, ZVelocity * DeltaTime);
		if(bFinishedMove)
		{
			DoorState = EDoorState::Opened;
			OnDoorOpened.Broadcast();
		}
	} else if(DoorState == Closing)
	{
		ZVelocity -= DoorGravity * DeltaTime;
		bool bFinishedMove = MoveDoorToPosition(DoorClosedPosition, ZVelocity * DeltaTime);
		if(bFinishedMove)
		{
			DoorState = EDoorState::Closed;
			OnDoorClosed.Broadcast(FMath::Abs(ZVelocity));
		}
	}
}

void UDoorMovement::SetupDoorMovement(USceneComponent* InDoor, FVector InOpenPosition, FVector InClosedPosition)
{
	Door = InDoor;
	DoorOpenedPosition = InOpenPosition;
	DoorClosedPosition = InClosedPosition;
}

void UDoorMovement::Open()
{
	DoorState = EDoorState::Opening;
}


void UDoorMovement::Close()
{
	DoorState = EDoorState::Closing;
	ZVelocity = 0;
}

