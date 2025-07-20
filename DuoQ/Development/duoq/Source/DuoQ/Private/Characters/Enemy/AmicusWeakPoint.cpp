#include "Characters/Enemy/AmicusWeakPoint.h"
#include "Engine/EngineTypes.h"

AAmicusWeakPoint::AAmicusWeakPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAmicusWeakPoint::BeginPlay()
{
	Super::BeginPlay();

	OnTakePointDamage.AddDynamic(this, &AAmicusWeakPoint::HandleTakePointDamage);
}

void AAmicusWeakPoint::HandleTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	if (GetAttachParentActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is a child and cannot take damage."), *GetName());
		return; // Exit the function without processing damage
	}

	Health -= Damage;
	if (Health <= 0.0f)
	{
		// Broadcast the delegate
		OnWeakPointDestroyed.Broadcast(WeakPointColor);

		// Destroy self
		Destroy();
	}
}
