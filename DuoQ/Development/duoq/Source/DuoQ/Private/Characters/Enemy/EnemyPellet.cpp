// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemyPellet.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyPellet::AEnemyPellet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);
	//CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	float XOffset = FMath::RandRange(GetActorLocation().X - 10.0f, GetActorLocation().X + 10.0f);
	FVector NewLoc = FVector(XOffset, GetActorLocation().Y, GetActorLocation().Z);
	SetActorLocation(NewLoc);


}

void AEnemyPellet::SetDamage(float Damage)
{
	PelletDamage = Damage;
}

// Called when the game starts or when spawned
void AEnemyPellet::BeginPlay()
{
	Super::BeginPlay();
	//GetCollisionComp()->IgnoreActorWhenMoving(this, true);
	
}

// Called every frame
void AEnemyPellet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

