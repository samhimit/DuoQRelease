// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemyShotgun.h"

#include "Characters/Enemy/EnemyPellet.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"

// Sets default values
AEnemyShotgun::AEnemyShotgun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemyShotgun::Fire()
{
	FVector const SpawnLoc = GetActorLocation();
	FRotator const SpawnRot = GetActorRotation();

	for (int i = 0; i < NumberOfPellets; i++)
	{
		ShootPellet(SpawnLoc, SpawnRot);
	}
}

// Called when the game starts or when spawned
void AEnemyShotgun::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyShotgun::ShootPellet(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	FVector Direction = SpawnRotation.Vector();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; 
	
	AEnemyPellet* Pellet = GetWorld()->SpawnActor<AEnemyPellet>(AEnemyPellet::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
	if (Pellet)
	{
		//if(AGruntShotgunEnemy* Enemy = Cast<AGruntShotgunEnemy>(GetOwner()))
		//{
			//?Pellet->SetDirection((UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation() - GetActorLocation()));

		//UE_LOG(LogTemp, Warning, TEXT("Pellet Spawned at: %s"), *SpawnLocation.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Pellet Direction: %s"), *Pellet->PelletDirection.ToString());
		//}
	}
}

// Called every frame
void AEnemyShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

