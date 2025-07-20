// Copyright 2024


#include "Characters/Partner/StarlightReactionComponent.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "GameSystems/DuoQCollectables.h"
#include "Perception/AISense_Sight.h"

// Sets default values for this component's properties
UStarlightReactionComponent::UStarlightReactionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStarlightReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner()) {
		StarlightAIController = Cast<AStarlightGOAPController>(Owner->GetInstigatorController());
		if (StarlightAIController) {
			PerceptionComponent = StarlightAIController->GetPerceptionComponent();
			if (PerceptionComponent) {
				PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &UStarlightReactionComponent::OnPerceptionUpdated);
			}
		}
	}
	
}


// Called every frame
void UStarlightReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStarlightReactionComponent::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);
	if (SenseClass == UAISense_Sight::StaticClass() && 
		Stimulus.WasSuccessfullySensed() &&
		Actor->IsA(ADuoQCollectables::StaticClass())) { 
        
		// Set focus first
		if (StarlightAIController) {
			StarlightAIController->SetFocus(Actor);
		}

		// UE_LOG(LogTemp, Warning, TEXT("Collectable detected, now shooting"));
		if (AStarlightCharacter* Character = Cast<AStarlightCharacter>(GetOwner())) {
			Character->StartShooting();

			// Stop shooting after set time
			GetWorld()->GetTimerManager().SetTimer(
					ShootingTimerHandle,
					this,
					&UStarlightReactionComponent::OnStopShooting,
					ShootingDuration,
					false
			);
			
			if(UDuoQStarlightWeapon* StarlightWeapon = Cast<UDuoQStarlightWeapon>(Character->Gun3P))
			{
				StarlightWeapon->TriggerUp();
			}
		}
	}
}

void UStarlightReactionComponent::OnStopShooting() const
{
	if (AStarlightCharacter* Character = Cast<AStarlightCharacter>(GetOwner()))
	{
		if (UDuoQStarlightWeapon* StarlightWeapon = Cast<UDuoQStarlightWeapon>(Character->Gun3P))
		{
			StarlightWeapon->TriggerUp();
			UE_LOG(LogTemp, Warning, TEXT("Stopped shooting after %.2f seconds"), ShootingDuration);
		}
	}
}

