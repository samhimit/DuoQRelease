// Copyright 2024


#include "Characters/Abilities/GunPickup.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "Components/SphereComponent.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"

// Sets default values
AGunPickup::AGunPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Visual"));
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AGunPickup::OverlapBegins);

}

void AGunPickup::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsForPlayer)
	{
		ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (OtherActor == MyCharacter)
		{
			Cast<ADuoQPlayerCharacter>(MyCharacter)->GetGunFromPickUp();
			UGameplayStatics::PlaySound2D(this, PickupSound);

			// I moved the save code into the GetGun equation (I forgot what a method/function is called) = samhji
			OnGunPickUp.Broadcast();
			Destroy();
		}
	}
	else
	{
		ACharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld());
		if (OtherActor == StarlightCharacter)
		{
			Cast<AStarlightCharacter>(StarlightCharacter)->GetGun();
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());

			UDuoQGameInstance* GameInstance = Cast<UDuoQGameInstance>(GetWorld()->GetGameInstance());
			USaveSubSystem*	   SaveSystem = GameInstance->GetSubsystem<USaveSubSystem>();
			SaveSystem->SetGun(false, true);
			OnGunPickUp.Broadcast();
			Destroy();
		}
	}
}