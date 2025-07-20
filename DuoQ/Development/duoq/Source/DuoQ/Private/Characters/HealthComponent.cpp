// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/HealthComponent.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Characters/Abilities/ElectricDamageType.h"
#include "GameFramework/Character.h"
#include "Characters/DynamicGOAPComponent.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "DuoQBlueprintFunctionLibrary.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	 // Initialize default values
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	RegenDelay = 5.0f;
	RegenRate = 1.0f;
	StunDuration = 2.0f;
	bCanRegen = true;
	bIsDead = false;
	bShielded = false;
	bHasShield = false;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the StunTag
	StunTag = FGameplayTag::RequestGameplayTag(FName("Status.Stun"));

    if (AActor* Owner = GetOwner())
	{
		// Bind the HandleTakePointDamage function to the OnTakePointDamage event
		Owner->OnTakePointDamage.AddDynamic(this, &UHealthComponent::HandleTakePointDamage);

		// Bind the HandleTakeRadialDamage function to the OnTakeRadialDamage event
		Owner->OnTakeRadialDamage.AddDynamic(this, &UHealthComponent::HandleTakeRadialDamage);
	}

	Audio = Cast<UAudioComponent>(StunLoop.GetComponent(GetOwner()));
}

void UHealthComponent::HandleDamage(float Damage, USceneComponent* HitComponent /*= nullptr*/, bool bApplyMultipliers /*= true*/, AActor* DamageCauser)
{
	// Apply damage multiplier if the hit component is in the map
	if (bApplyMultipliers && HitComponent && DamageMultipliers.Contains(HitComponent))
	{
		Damage *= DamageMultipliers[HitComponent];
	}

	CurrentHealth = CurrentHealth - Damage;

	if (HurtSfx)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HurtSfx, GetOwner()->GetActorLocation());
	}

	// Broadcast the health update event
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	AGruntShotgunEnemy* OwningGrunt = Cast<AGruntShotgunEnemy>(GetOwner());
	if (OwningGrunt)
	{
		// Broadcast the delegate with the owning grunt and new health
		OnGruntHealthChanged.Broadcast(OwningGrunt, CurrentHealth);
	}

	if (0 < CurrentHealth && CurrentHealth <= (MaxHealth * LowHealthRatio))
	{
		OnLowHealth.Broadcast();
		// Play the low health sound effect if not playing already
		if (LowHealthLoopSfx && Audio)
		{
			if (Audio->Sound.Get() != LowHealthLoopSfx)
			{
				Audio->Stop();
				Audio->SetSound(LowHealthLoopSfx);
				Audio->Play();

				if (LowHealthSfx)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), LowHealthSfx);
				}
			}

			Audio->SetFloatParameter("Gain", LowHealthLoopGain.GetRichCurve()->Eval(CurrentHealth));
		}
	}

	else if (CurrentHealth <= 0)
	{
		HandleDeath();
		OnDeathWithKiller.Broadcast(DamageCauser);
	}

	if (bCanRegen && CurrentHealth < MaxHealth)
	{
		// Stop the regen sound if hurt
		if (Audio)
		{
			if (Audio->Sound.Get() == HealthRegenSfx)
			{
				Audio->Stop();
				Audio->SetSound(nullptr);
			}
		}
		bIsRegening = false;
		GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle); // Clear any existing timer
		GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &UHealthComponent::HandleRegeneration, RegenDelay, false);
	}

}

//Returns the Actor's current Health
float UHealthComponent::GetCurrentHealth()
{
	return CurrentHealth;
}


// Function to check if another HealthComponent is friendly
bool UHealthComponent::IsFriendlyWith(const UHealthComponent* OtherHealthComponent) const
{
	return UDuoQBlueprintFunctionLibrary::GetTeamAttitudeTowards(GetOwner(),OtherHealthComponent->GetOwner()) == ETeamAttitude::Friendly;
}

void UHealthComponent::HandleRegeneration()
{
	//let health component know to start regenning
	bIsRegening = true;

	if (CurrentHealth < MaxHealth)
	{
		// Start the regen sound effect if not already playing
		if (HealthRegenSfx && Audio)
		{
			if (Audio->Sound.Get() != HealthRegenSfx)
			{
				Audio->Stop();
				Audio->SetSound(HealthRegenSfx);
				Audio->SetFloatParameter("Health", CurrentHealth);
				Audio->Play();
			}
			else
			{
				Audio->SetFloatParameter("Health", CurrentHealth);
			}
		}
	}
	// If fully recovered, stop the regen loop
	else if (bCanPlayRegenSound && HealthRegenSfx && Audio)
	{
		if (Audio->Sound.Get() == HealthRegenSfx)
		{
			Audio->Stop();
			Audio->SetSound(nullptr);
	
			if (HealthRegenStopSfx)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), HealthRegenStopSfx);
			}
		}
	}
	
	
	// // Increase current health by the regeneration rate
	// CurrentHealth += RegenRate;
	//
	// // Clamp the current health to ensure it does not exceed the maximum health
	// CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	//
	// // Broadcast the health update event
	// OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	//
	// if (CurrentHealth < MaxHealth)
	// {
	// 	// Set timer to call this function again after 0.5 seconds
	// 	GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &UHealthComponent::HandleRegeneration, 0.5f, false);
	// 	// Start the regen sound effect if not already playing
	// 	if (HealthRegenSfx && Audio)
	// 	{
	// 		if (Audio->Sound.Get() != HealthRegenSfx)
	// 		{
	// 			Audio->Stop();
	// 			Audio->SetSound(HealthRegenSfx);
	// 			Audio->SetFloatParameter("Health", CurrentHealth);
	// 			Audio->Play();
	// 		}
	// 		else
	// 		{
	// 			Audio->SetFloatParameter("Health", CurrentHealth);
	// 		}
	// 	}
	// }
	// // If fully recovered, stop the regen loop
	// else if (bCanPlayRegenSound && HealthRegenSfx && Audio)
	// {
	// 	if (Audio->Sound.Get() == HealthRegenSfx)
	// 	{
	// 		Audio->Stop();
	// 		Audio->SetSound(nullptr);
	//
	// 		if (HealthRegenStopSfx)
	// 		{
	// 			UGameplayStatics::PlaySound2D(GetWorld(), HealthRegenStopSfx);
	// 		}
	// 	}
	// }

}

void UHealthComponent::TickRegeneration(float DeltaTime)
{
	CurrentHealth += RegenRate * DeltaTime;

	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	
	// Broadcast the health update event
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth < MaxHealth)
	{
		// Start the regen sound effect if not already playing
		if (HealthRegenSfx && Audio)
		{
			if (Audio->Sound.Get() != HealthRegenSfx)
			{
				Audio->Stop();
				Audio->SetSound(HealthRegenSfx);
				Audio->SetFloatParameter("Health", CurrentHealth);
				Audio->Play();
			}
			else
			{
				Audio->SetFloatParameter("Health", CurrentHealth);
			}
		}
	}
	// If fully recovered, stop the regen loop
	else if (bCanPlayRegenSound && HealthRegenSfx && Audio)
	{
		if (Audio->Sound.Get() == HealthRegenSfx)
		{
			Audio->Stop();
			Audio->SetSound(nullptr);

			if (HealthRegenStopSfx)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), HealthRegenStopSfx);
			}
		}
	}
	
}


void UHealthComponent::HandleTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}

	if (DamageCauser)
	{
		// Get the HealthComponent of the DamageCauser
		UHealthComponent* CauserHealthComponent = Cast<UHealthComponent>(DamageCauser->GetComponentByClass(UHealthComponent::StaticClass()));

		// Check for friendly fire
		if (CauserHealthComponent && IsFriendlyWith(CauserHealthComponent))
		{
			// Prevent friendly actors from damaging each other
			return;
		}

		// check if the damage is inflicted by the player
		if (Cast<ADuoQPlayerCharacter>(DamageCauser))
		{
			if (DamagedActor)
			{
				if (UDynamicGOAPComponent* DGOAP = DamagedActor->GetComponentByClass<UDynamicGOAPComponent>())
				{
					DGOAP->UpdateKeyedAtomStatus(FString(TEXT("IsDamageable")), true);
					// also wanna run a linetrace here to see if we want to update the InSight tag since AI Perception is a little fucked
					TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Camera;
					FCollisionQueryParams		   QueryParams;
					AStarlightCharacter*		   Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this);
					QueryParams.AddIgnoredActor(Starlight);
					ACharacter* Enemy = Cast<ACharacter>(GetOwner()); 
					FVector	   TargetHead = Enemy->GetMesh()->GetBoneLocation(FName("crest"));
					FHitResult HitResult;
					
					if (GetWorld()->LineTraceSingleByChannel(HitResult, Starlight->GetActorLocation(), TargetHead, TraceChannelProperty, QueryParams))
					{
						// insight is true
						if (HitResult.GetActor() == Enemy)
						{
							DGOAP->UpdateKeyedAtomStatus(FString(TEXT("InSight")), true);
						}
					}

					if (AStarlightGOAPController* SGOAP = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(this))
					{
						SGOAP->SetFocus(DamagedActor);
					}
				}
			}
		}
	}

	// important to apply status afterwords so that we don't stun dead enemies or damage a just broken shield
	if (DamageType->IsA(UElectricDamageType::StaticClass()))
	{
		if (!bShielded)
		{
			//shouldn't actually deal damage if electric type... this is a hack because apply damage swallows any damage with a value of 0
			HandleDamage(0, HitComponent, true, DamageCauser);
			ApplyStun();

			if (bHasShield)
			{
				GetWorld()->GetTimerManager().ClearTimer(ShieldRegenTimerHandle);
				GetWorld()->GetTimerManager().SetTimer(ShieldRegenTimerHandle, this, &UHealthComponent::ApplyShield, StunDuration + RechargeTime, false);
			}
		}
		
		else if (bShielded && bHasShield)
		{	
			bShielded = false;
			OnShieldBreak.Broadcast();

			AGruntShotgunEnemy* OwningGrunt = Cast<AGruntShotgunEnemy>(GetOwner());
			if (OwningGrunt)
			{
				// Broadcast the delegate with the owning grunt and new health
				OnGruntHealthChanged.Broadcast(OwningGrunt, CurrentHealth);
			}

			GetWorld()->GetTimerManager().ClearTimer(ShieldRegenTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(ShieldRegenTimerHandle, this, &UHealthComponent::ApplyShield, RechargeTime, false);
		}
	}

	else if (!bShielded)
	{
		HandleDamage(Damage, HitComponent, true, DamageCauser);
	}
}

// Event handler for when the component's owner takes radial damage
void UHealthComponent::HandleTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector HitLocation, FHitResult const& HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}

	if (DamageCauser)
	{
		// Get the HealthComponent of the DamageCauser
		UHealthComponent* CauserHealthComponent = Cast<UHealthComponent>(DamageCauser->GetComponentByClass(UHealthComponent::StaticClass()));

		// Check for friendly fire
		if (CauserHealthComponent && IsFriendlyWith(CauserHealthComponent))
		{
			// Prevent friendly actors from damaging each other
			return;
		}
	}

	// important to apply status afterwords so that we don't stun dead enemies or damage a just broken shield
	if (DamageType->IsA(UElectricDamageType::StaticClass()))
	{
		if (!bShielded)
		{
			//shouldn't actually deal damage if electric type... this is a hack because apply damage swallows any damage with a value of 0
			HandleDamage(0, nullptr, false, DamageCauser);
			ApplyStun();

			if (bHasShield)
			{
				GetWorld()->GetTimerManager().ClearTimer(ShieldRegenTimerHandle);
				GetWorld()->GetTimerManager().SetTimer(ShieldRegenTimerHandle, this, &UHealthComponent::ApplyShield, StunDuration + RechargeTime, false);
			}
		}

		else if (bShielded && bHasShield)
		{
			bShielded = false;
			OnShieldBreak.Broadcast();

			AGruntShotgunEnemy* OwningGrunt = Cast<AGruntShotgunEnemy>(GetOwner());
			if (OwningGrunt)
			{
				// Broadcast the delegate with the owning grunt and new health
				OnGruntHealthChanged.Broadcast(OwningGrunt, CurrentHealth);
			}

			GetWorld()->GetTimerManager().ClearTimer(ShieldRegenTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(ShieldRegenTimerHandle, this, &UHealthComponent::ApplyShield, RechargeTime, false);
		}
	}

	else if (!bShielded)
	{
		HandleDamage(Damage, nullptr, false, DamageCauser);
	}
}




// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsRegening)
	{
		TickRegeneration(DeltaTime);
	}
	
}

// Function to add a component to the damage multipliers map
void UHealthComponent::AddDamageMultiplier(USceneComponent* Component, float Multiplier)
{
	if (Component)
	{
		DamageMultipliers.Add(Component, Multiplier);
	}
}

void UHealthComponent::AddStatusEffect(FGameplayTag StatusEffect)
{
	if (!ActiveStatusEffects.HasTag(StatusEffect))
	{
		ActiveStatusEffects.AddTag(StatusEffect);
		// Handle the effect of this status, e.g., disable player input if stunned
	}
}

void UHealthComponent::RemoveStatusEffect(FGameplayTag StatusEffect)
{
	if (ActiveStatusEffects.HasTag(StatusEffect))
	{
		ActiveStatusEffects.RemoveTag(StatusEffect);
		// Handle the removal of this status, e.g., enable player input if previously stunned
	}
}

bool UHealthComponent::HasStatusEffect(FGameplayTag StatusEffect) const
{
	return ActiveStatusEffects.HasTag(StatusEffect);
}

void UHealthComponent::ApplyStun() 
{
	// Implement logic to freeze the actor
	// For example, disabling movement, applying a stunned status, etc.
	OnStun.Broadcast();
	if (!HasStatusEffect(StunTag))
	{
		AddStatusEffect(StunTag);

        // Logic to freeze the actor
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			Character->GetCharacterMovement()->DisableMovement();
			UGameplayStatics::PlaySoundAtLocation(Character, StunStartSfx, Character->GetActorLocation());
			if (Audio)
			{
				Audio->Play();
			}
		}

		//Set a timer to unstun after a certain duration
		GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &UHealthComponent::RemoveStun, StunDuration, false);
	}
}

void UHealthComponent::RemoveStun()
{
	OnRemoveStun.Broadcast();
	if (StunTag.IsValid())
	{
		RemoveStatusEffect(StunTag);

		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			// Re-enable movement
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			UGameplayStatics::PlaySoundAtLocation(Character, StunEndSfx, Character->GetActorLocation());
			if (Audio)
			{
				Audio->FadeOut(0.5f, 0.0f);
			}
		}
	}
}

void UHealthComponent::HandleDeath() 
{
	OnDeath.Broadcast();
	if (!bIsDead) OnDeathWithSource.Broadcast(GetOwner()->GetActorLocation());
	bIsDead = true;

	ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		PlayerCharacter->DisableMovementOnDeath();
	}

	if (Audio)
	{
		Audio->Stop();
		Audio->SetSound(nullptr);
	}

	bCanPlayRegenSound = false;
}

void UHealthComponent::Revive() 
{
	bIsDead = false;
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	GetWorld()->GetTimerManager().SetTimer(ReviveRegenSoundTimerHandle, this, &UHealthComponent::ResetRegenSound, RegenDelay + RegenRate, false);
}

void UHealthComponent::ApplyShield()
{
	if (bIsDead)
	{
		return;
	}

	bShielded = true;
	OnShieldRecharge.Broadcast();
}

void UHealthComponent::ResetRegenSound()
{
	bCanPlayRegenSound = true;
	if (HealthRegenSfx && Audio)
	{
		if (Audio->Sound.Get() == HealthRegenSfx)
		{
			Audio->Stop();
			Audio->SetSound(nullptr);
		}
	}
}
