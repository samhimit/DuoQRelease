
#include "Characters/Abilities/DuoQElectricRevolver.h"
#include "Characters/Abilities//DuoQBullet.h"
#include "Characters/Abilities/RegularDamageType.h"
#include "Characters/Abilities/ElectricDamageType.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "UI/AbilityWidget.h"
#include "UI/GameplayHUD.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/GunChargeWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "UI/DuoQHUD.h"

UDuoQElectricRevolver::UDuoQElectricRevolver()
{
	WindDownTime = 1.0f;
}

void UDuoQElectricRevolver::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(GetWorld()->GetFirstPlayerController()->InputComponent))
	{
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &UDuoQElectricRevolver::TriggerDown);
	}
}

void UDuoQElectricRevolver::SpawnElectricProjectile()
{
	ADuoQBullet* bullet;
	if ((bullet = static_cast<ADuoQBullet*>(FireProjectile(ChargedProjectileClass, ChargedShot))) != nullptr)
	{
		bullet->InitParameter(5, GetOwner()->GetInstigatorController(), GetOwner(), UElectricDamageType::StaticClass());
		//UNiagaraFunctionLibrary::SpawnSystemAttached(ElectricBulletParticles, bullet->GetRootComponent(), TEXT("NONE"), FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);			
	}
	
	OnBulletFired.Broadcast(ChargedProjectileClass);
}

void UDuoQElectricRevolver::UpdateState(EPlayerGunState NewState)
{
	GunState = NewState;
	OnGunStateUpdated.Broadcast(NewState);
	StateTimer = 0;
}

void UDuoQElectricRevolver::WeaponTick(float deltaTime)
{
	
}

void UDuoQElectricRevolver::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	StateTimer += DeltaTime;
	if(GunState == WindingDown)
	{
		if(StateTimer >= WindDownTime)
		{
			UpdateState(ReadyToFire);
		}
	}
}

void UDuoQElectricRevolver::TriggerDown()
{
	if (!bIsUsable)
		return;

	if(GunState != ReadyToFire)
	{
		return;
	}

	SpawnElectricProjectile();
	UpdateState(WindingDown);
	
}
