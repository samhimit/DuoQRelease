// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/Deprecated/DuoQLightBeam.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ADuoQLightBeam::ADuoQLightBeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CollisionComp->InitCapsuleSize(InRadius, HalfHeight);
	CollisionComp->BodyInstance.SetCollisionProfileName("LightBeam");

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

}

void ADuoQLightBeam::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ADuoQPlayerCharacter>(OtherActor))
	{
		//reflect off shield
	}
	if (ADuoQEnemyCharacter* Enemy = Cast<ADuoQEnemyCharacter>(OtherActor))
	{
		//damage enemy
	}
}
// Called when the game starts or when spawned
void ADuoQLightBeam::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	
}

// Called every frame
void ADuoQLightBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

