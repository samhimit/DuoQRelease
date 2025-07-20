// Copyright Epic Games, Inc. All Rights Reserved.
#include "Characters/Abilities/DuoQBullet.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/HealthComponent.h"
#include "Characters/Abilities/ElectricDamageType.h"
#include "Characters/Abilities/LinecastProjectile.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"

ADuoQBullet::ADuoQBullet()
{
	// Use a sphere as a simple collision representation
	LinecastComp = CreateDefaultSubobject<ULinecastProjectile>(TEXT("LinecastProjectile"));
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//LinecastComp->RegisterComponent();
	// CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	// CollisionComp->InitSphereRadius(0.1f);
	// CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// CollisionComp->OnComponentHit.AddDynamic(this, &ADuoQBullet::OnHit); // set up a notification for when this component hits something blocking
	// CollisionComp->SetSimulatePhysics(false);
	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Visual"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SphereVisual->SetWorldScale3D(FVector(0.1f));
	}

	DelayDestroyLifetime = 0.0f;

	bLetItselfHandleDestroy = false;

	// Players can't walk on it
	// CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	// CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	//RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	//ProjectileMovement->UpdatedComponent = LinecastComp;
	ProjectileMovement->InitialSpeed = 20000.f;
	ProjectileMovement->MaxSpeed = 20000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}
void ADuoQBullet::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(3.0f);
	bProjectilePendingDestroy = false;
}

void ADuoQBullet::InitParameter(int damage, AController* instigator, AActor* source, TSubclassOf<UDamageType> damageType)
{
	mDamage = damage;
	mDamageType = damageType;
	mInstigator = instigator;
	mSource = source;
	
	if (damageType && damageType->IsChildOf(UElectricDamageType::StaticClass()))
	{
		/**
		UPointLightComponent* PointLight = NewObject<UPointLightComponent>(this, TEXT("Electric Illumination"));
		
		// Set light properties
		PointLight->SetIntensity(2000.0f);
		PointLight->SetLightColor(FLinearColor(0.3,0.5,1));
		PointLight->SetAttenuationRadius(2000.0f);

		// Attach to the bullet's root component
		PointLight->SetupAttachment(RootComponent);
		PointLight->RegisterComponent();
		*/
	}
}

void ADuoQBullet::LifeSpanExpired()
{
	if (DelayDestroyLifetime > 0.0f)
	{
		bProjectilePendingDestroy = true;
		DelayDestroyTimer = DelayDestroyLifetime;
	} else
	{
		Destroy();
	}
}

void ADuoQBullet::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bProjectilePendingDestroy)
	{
		DelayDestroyLifetime -= DeltaSeconds;
		if (DelayDestroyLifetime <= 0.0f)
		{
			Destroy();
		}
		
	}
}

void ADuoQBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (bProjectilePendingDestroy) return;

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (OtherActor != mSource) && (mInstigator != nullptr) && (OtherActor != mInstigator->GetPawn()))
	{
		UE_LOG(LogTemp, Display, TEXT("Hit %s"), *OtherActor->GetName());
		UE_LOG(LogTemp, Display, TEXT("Dealing %i Damage to %s"), mDamage, *OtherComp->GetName());
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		float multiplier = 1.0f;
		if (Hit.BoneName == "head")
		{
			multiplier = 1.5f;
		}


		FVector HitLocation = Hit.Location;
		FVector Direction = ProjectileMovement->Velocity;
		Direction.Normalize();
		HitLocation -= Direction * PenetrationPushOut;

		
		UGameplayStatics::ApplyPointDamage(OtherActor, mDamage * multiplier, GetVelocity(), Hit, mInstigator, mSource, mDamageType);
		if (mDamageType == UElectricDamageType::StaticClass())
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ElectricDamageParticleSystem, HitLocation);
		}
		else
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RegularDamageParticleSystem, HitLocation);
		}
		if (this)
		{
			if (DelayDestroyLifetime > 0.0f)
			{
				bProjectilePendingDestroy = true;
				DelayDestroyTimer = DelayDestroyLifetime;
				ProjectileMovement->StopMovementImmediately();
			} else
			{
				Destroy();
			}
			
		}
 	}
}