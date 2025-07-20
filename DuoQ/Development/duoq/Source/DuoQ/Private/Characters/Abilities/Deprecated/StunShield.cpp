// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/Deprecated/StunShield.h"

#include "Characters/Abilities/DuoQBullet.h"
#include "Characters/Abilities/ElectricDamageType.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
AStunShield::AStunShield()
{
	//initializing mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComp;
	
	// initializing collision
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComp->SetupAttachment(RootComponent);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionComp->InitSphereRadius(100.0f);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AStunShield::OnBeginOverlap);
}


// Called when the game starts
void AStunShield::BeginPlay()
{
	Super::BeginPlay();
	
}

void AStunShield::EnableShield()
{
	bIsDisabled = false;
	MeshComp->SetVisibility(true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AStunShield::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}


void AStunShield::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ADuoQBullet* const Bullet = Cast<ADuoQBullet>(OtherActor))
	{
		// checking if type is electric
		if(Bullet->GetDamageType() == UElectricDamageType::StaticClass())
		{
			bIsDisabled = true;
			MeshComp->SetVisibility(false);
			MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetWorld()->GetTimerManager().SetTimer(DisableTimerHandle, this, &AStunShield::EnableShield, RechargeTime, false);
		}
	}
}

