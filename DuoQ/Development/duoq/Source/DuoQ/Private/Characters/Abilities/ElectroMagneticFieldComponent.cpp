// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Abilities/ElectroMagneticFieldComponent.h"
#include "Characters/Abilities/RegularDamageType.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Characters/Abilities/MagnetizableRocket.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
AElectroMagneticFieldComponent::AElectroMagneticFieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Visual"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("MaterialInterface'/Game/Characters/Abilities/CaptureFieldAbility/M_TempElectroMagneticField'"));
	SphereVisual->SetRelativeScale3D(FVector::OneVector * 0.2f);
	SphereVisual->SetStaticMesh(SphereMeshAsset.Object);
	SphereVisual->SetMaterial(0, MaterialAsset.Object);
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Looping Audio"));
	
	SetActorTickEnabled(true);
	Deactivate();
	// ...
}

void AElectroMagneticFieldComponent::BeginPlay()
{
	Super::BeginPlay();
	SphereVisual->OnComponentBeginOverlap.AddDynamic(this, &AElectroMagneticFieldComponent::OnOverlap);
}

void AElectroMagneticFieldComponent::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("Overlapping with %s"), *(OtherActor->GetName()));
	if (dynamic_cast<AMagnetizableRocket*>(OtherActor))
	{
		UE_LOG(LogTemp, Display, TEXT("Right Object"));
		AMagnetizableRocket* magComp = static_cast<AMagnetizableRocket*>(OtherActor);
		StoreProjectile(magComp);
	}
}

void AElectroMagneticFieldComponent::Activate()
{
	IsActiveField = true;
	SetActorEnableCollision(true);
	SphereVisual->SetVisibility(true, true);	//TODO: Add Looping Audio
	//UGameplayStatics::PlaySound2D(this, ActivateSound);
	AudioComp->Play();
}
void AElectroMagneticFieldComponent::Deactivate()
{
	IsActiveField = false;
	ReleaseProjectile();
	SetActorEnableCollision(false);
	SphereVisual->SetVisibility(false, true);
	TArray<AActor*> arr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMagnetizableRocket::StaticClass(), arr);
	for (AActor* a : arr)
	{
		static_cast<AMagnetizableRocket*>(a)->RemoveMagnetizableSource();
	}

	AudioComp->SetTriggerParameter(TEXT("Stop"));
	AudioComp->StopDelayed(2.0f);
	//UGameplayStatics::PlaySound2D(this, DeactivateSound);
}

void AElectroMagneticFieldComponent::StoreProjectile(AMagnetizableRocket* projectile)
{
	Rocket = projectile;
	InReboundingState = true;
	UGameplayStatics::PlaySound2D(this, AbsorbSound);
	UProjectileMovementComponent* moveComp = projectile->ProjectileMovement;
	moveComp->SetComponentTickEnabled(false);
	projectile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	projectile->SetActorRelativeLocation(FVector::Zero());
	projectile->SetMagnetizableSource(SphereVisual);
}

void AElectroMagneticFieldComponent::ReleaseProjectile()
{
	if (Rocket)
	{
		Rocket->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		UProjectileMovementComponent* moveComp = Rocket->ProjectileMovement;
		moveComp->SetComponentTickEnabled(true);
		moveComp->Velocity = GetActorForwardVector() * moveComp->MaxSpeed; 
		Rocket->RemoveMagnetizableSource();
		Rocket = nullptr;
		UGameplayStatics::PlaySound2D(this, ReleaseSound);
	}
}