
#include "Characters/Abilities/DuoQStarlightWeapon.h"
#include "Characters/Abilities/DuoQBullet.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Characters/Abilities/RegularDamageType.h"
#include "Kismet/GameplayStatics.h"

void UDuoQStarlightWeapon::WeaponTick(float deltaTime)
{
	StateTimer += deltaTime;
	switch (GunState)
	{
		case ShootingState::Shooting:
			ShootingTime += deltaTime;
			if (StateTimer > FireRate)
			{
				StateTimer = 0.0f;
				if (CurrentMag <= 0)
				{
					UE_LOG(LogTemp, Display, TEXT("Starting Reloading"));
					// Reload();
					//  since starlight reloading is pertinent to GOAP (So she'll consider meleeing) we'll make Starlight always manually reload
					OnMagazineEmpty.Broadcast();
					GunState = ShootingState::Idle;
					break;
				}
				if (ADuoQBullet* DuoQBullet = FireProjectile(ProjectileClass, BloomConeAngle))
				{
					DuoQBullet->InitParameter(Damage, GetOwner()->GetInstigatorController(), GetOwner(), URegularDamageType::StaticClass());
					UNiagaraFunctionLibrary::SpawnSystemAttached(BulletParticleSystem, DuoQBullet->GetRootComponent(), TEXT("NAME_None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
					OnBulletFired.Broadcast(ProjectileClass);
					if (ShootingSound)
					{
						// Pass in the amount of time Starlight has been shooting (representing her "ammo")
						ShootingSound->SetFloatParameter("LayerGain", ShootingLayerGain.GetRichCurve()->Eval(CurrentMag));
						ShootingSound->SetFloatParameter("PitchShift", ShootingPitchShift.GetRichCurve()->Eval(CurrentMag));
					}
					CurrentMag--;
				}
			}
			break;
		case ShootingState::Reloading:
			if (StateTimer >= ReloadSpeed)
			{
				StateTimer = 0.0f;
				CurrentMag = MagSize;
				UE_LOG(LogTemp, Display, TEXT("Reloaded"));
				OnReloadFinish.Broadcast();
				GunState = IsHoldingTrigger ? ShootingState::Shooting : ShootingState::Idle;
				if (GunState == ShootingState::Shooting)
				{
					if (ShootingSound)
					{
						if (!ShootingSound->IsPlaying())
						{
							ShootingSound->Play();
						}
						ShootingSound->SetTriggerParameter(TEXT("StartShooting"));
					}
				}
			}
		case ShootingState::Idle:
			break;
	}
}

void UDuoQStarlightWeapon::Reload()
{
	GunState = ShootingState::Reloading;
	UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetOwner()->GetActorLocation());
	OnReload.Broadcast();
	// Add other stuff for reload like animation trigger.
}

void UDuoQStarlightWeapon::BeginPlay()
{
	Super::BeginPlay();

	// bind fire anim function to OnBulletFired Delegate
	OnBulletFired.AddDynamic(this, &UDuoQStarlightWeapon::HandleOnBulletFired);
	ShootingSound = Cast<UAudioComponent>(ShootingAudioComponent.GetComponent(GetOwner()));
	CurrentMag = MagSize;
}

void UDuoQStarlightWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnBulletFired.RemoveAll(this);
}

void UDuoQStarlightWeapon::HandleOnBulletFired(TSubclassOf<class ADuoQBullet> projectile)
{
	if (AStarlightCharacter* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this))
	{
		if (USkeletalMeshComponent* SMC = Starlight->GetMesh())
		{
			// play shooting montage
			SMC->GetAnimInstance()->Montage_Play(ShootingAnim);
			
			// shooting vfx
			UE_LOG(LogTemp, Display, TEXT("Socket Found At: %s"), *GetSocketTransform(FName("Muzzle"), RTS_Actor).GetLocation().ToString());
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ShootingParticleSystem, GetSocketLocation(FName("Muzzle")),*(new FRotator(0.0f,90.0f,0.0f)));
		}
	}
}

void UDuoQStarlightWeapon::TriggerUp()
{
	OnTriggerUp.Broadcast();
	IsHoldingTrigger = false;

	// stop shooting sound
	if (ShootingSound && ShootingSound->IsPlaying())
	{
		if (ShootingTime > 0.1f)
		{
			ShootingSound->SetTriggerParameter(TEXT("StopShooting"));
		}
		else
		{
			// Prevent random rifle tails from playing without an actual shot first
			ShootingSound->Stop();
		}
	}
	ShootingTime = 0.0f;

	if (GunState == ShootingState::Reloading)
		return;
	StateTimer = 0.0f;
	
	GunState = ShootingState::Idle;
}

void UDuoQStarlightWeapon::TriggerDown()
{
	if (!bIsUsable)
		return;
	OnTriggerDown.Broadcast();
	IsHoldingTrigger = true;
	if (GunState == ShootingState::Reloading)
		return;
	StateTimer = 0.0f;
	ShootingTime = 0.0f;
	GunState = ShootingState::Shooting;
	// play shooting sound
	if (ShootingSound)
	{
		if (!ShootingSound->IsPlaying())
		{
			ShootingSound->Play();
		}
		ShootingSound->SetTriggerParameter(TEXT("StartShooting"));
	}
}