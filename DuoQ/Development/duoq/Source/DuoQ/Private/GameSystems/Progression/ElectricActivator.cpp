// Copyright 2024


#include "GameSystems/Progression/ElectricActivator.h"
#include "Characters/Abilities/ElectricDamageType.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AElectricActivator::AElectricActivator()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
}

void AElectricActivator::BeginPlay()
{
	Super::BeginPlay();
	OnTakePointDamage.AddDynamic(this, &AElectricActivator::TakePointDamage);
}
void AElectricActivator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}
void AElectricActivator::TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	if (Cast<UElectricDamageType>(DamageType))
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[this]() {
				if (CurrentCharge >= MaxCharge)
				{
					CurrentCharge = MaxCharge;
					if (!bIsTriggered)
					{
						ActivateTrigger(GetActorLocation());
						bIsTriggered = true;
					}
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle,
						[this]() {
							if (CurrentCharge <= 0.0f)
							{
								bIsTriggered = false;
								CurrentCharge = 0.0f;
								DeactivateTrigger();
								AudioComponent->FadeOut(0.5f, 0.0f);
								UGameplayStatics::PlaySoundAtLocation(this, DeactivateSFX, GetActorLocation());
								GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
								return;
							}
							CurrentCharge -= DecreaseRate;
							UpdateCharge(CurrentCharge / MaxCharge);
						},
						0.1f,
						true);
					return;
				}
				CurrentCharge += IncreaseRate;
				UpdateCharge(CurrentCharge / MaxCharge);
				AudioComponent->FadeIn(0.5f);
				UGameplayStatics::PlaySoundAtLocation(this, ActivateSFX, GetActorLocation());
			},
			0.1f,
			true);
	}
}