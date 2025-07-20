// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/MagnetizableRocket.h"

#include "Characters/Abilities/LinecastProjectile.h"
#include "Characters/Abilities/RegularDamageType.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/AudioComponent.h"

// Sets default values for this component's properties
AMagnetizableRocket::AMagnetizableRocket()
{
	//CollisionComp->InitSphereRadius(0.2f);
	SphereVisual->SetWorldScale3D(FVector(0.3f));
	ProjectileMovement->InitialSpeed = 500.0f;
	ProjectileMovement->MaxSpeed = 500.0f;

	RocketLoop = CreateDefaultSubobject<UAudioComponent>(TEXT("RocketLoop"));
	RocketLoop->SetupAttachment(SphereVisual);
}

void AMagnetizableRocket::BeginPlay()
{
	Super::BeginPlay();
	//LinecastComp->OnLineHit.Clear();
	//LinecastComp->OnLineHit.AddDynamic(this, &AMagnetizableRocket::ProjectileStop);
	SetLifeSpan(50.0f);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RocketRelease, GetActorLocation());
}

void AMagnetizableRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
	ADuoQEnemyCharacter* enemy = static_cast<ADuoQEnemyCharacter*>(OtherActor);
	if (enemy)
	{
		APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		TArray<AActor*>	   arr;
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, Hit.ImpactPoint, ExplosionRadius, URegularDamageType::StaticClass(), arr, DamageCauser, controller);
		ProjectileMovement->StopMovementImmediately();
	}

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RocketExplosion, GetActorLocation());
	UE_LOG(LogTemp, Display, TEXT("Destroyed by colliding with %s"), *(OtherActor->GetActorNameOrLabel()));
	Destroy();
}