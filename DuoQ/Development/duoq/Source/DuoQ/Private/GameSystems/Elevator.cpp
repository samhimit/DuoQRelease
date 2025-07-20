// Copyright 2024


#include "GameSystems/Elevator.h"

#include "DuoQBlueprintFunctionLibrary.h"

// Sets default values
AElevator::AElevator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MoveDistance = 460.0f;
	MoveSpeed = 0.5;
	bShouldMove = false;
	bPlayerInBox = false;
	bStarlightInBox = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ElevatorTrigger"));
	if(TriggerBox)
	{
		TriggerBox->SetupAttachment(RootComp);
	}
	
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();

	bIsAtStart = true;

	Start = GetActorLocation();

	// Find the Box Collider by name
	UBoxComponent* FoundBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Box")));
	if (FoundBox)
	{
		BoxCollider = FoundBox;
		BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AElevator::OnComponentBeginOverlap);
		BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AElevator::OnComponentEndOverlap);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BoxCollider (Box) not found in AElevator!"));
	}
}

// Tick
void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Overlap Begin
void AElevator::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OverlappedComponent == TriggerBox)
	{
		if (OtherActor->IsA(ADuoQPlayerCharacter::StaticClass()))
		{
			bPlayerInBox = true;
		}
		else if (OtherActor->IsA(AStarlightCharacter::StaticClass()))
		{
			bStarlightInBox = true;
		}
	}

	if(bPlayerInBox && bStarlightInBox)
	{
		//Instead of straight up moving elevator we will fire event and wait for door to close first lmao
		OnBothPlayersInElevator.Broadcast();
	}
}

// Overlap End
void AElevator::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OverlappedComponent == TriggerBox)
	{
		if (OtherActor->IsA(ADuoQPlayerCharacter::StaticClass()))
		{
			bPlayerInBox = false;
		}
		else if (OtherActor->IsA(AStarlightCharacter::StaticClass()))
		{
			bStarlightInBox = false;
		}
	}
}


// Move the elevator
void AElevator::MoveElevator()
{
	bIsAtStart = !bIsAtStart;
	
	FVector LocationToTeleportTo = bIsAtStart ? End : Start;

	FVector Displacement = LocationToTeleportTo - GetActorLocation();
	
	SetActorLocation(LocationToTeleportTo, false);

	AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	AActor* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld());

	Player->AddActorWorldOffset(Displacement);
	Starlight->AddActorWorldOffset(Displacement);

	OnElevatorMoved.Broadcast();
}