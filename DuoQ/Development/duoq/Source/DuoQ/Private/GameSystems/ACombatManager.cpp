// Copyright 2024


#include "GameSystems/ACombatManager.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/HealthComponent.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"
#include "GameSystems/Dialogue/DialogueTriggerComponent.h"
#include "GameSystems/Audio/AudioSubsystem.h"
#include "GameSystems/Audio/MusicManager.h"
#include "GameSystems/Progression/TriggerTarget.h"

// Sets default values
ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = true;
	TickInterval = 3;

	StartOfCombat = CreateDefaultSubobject<USceneComponent>(TEXT("StartOfCombat"));
	if(StartOfCombat)
	{
		StartOfCombat->SetupAttachment(GetRootComponent());
	}

	DialogueTriggerComponent = CreateDefaultSubobject<UDialogueTriggerComponent>(TEXT("DialogueTriggerComponent"));
}

// BeginPlay
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();

	DuoQGameMode = Cast<ADuoQGameMode>(GetWorld()->GetAuthGameMode());

	for (AGruntShotgunEnemy* Grunt : Grunts)
	{
		if (Grunt)
		{
			UHealthComponent* HealthComponent = Grunt->FindComponentByClass<UHealthComponent>();
			if (HealthComponent)
			{
				HealthComponent->OnDeath.AddDynamic(this, &ACombatManager::HandleGruntDeath);
				HealthComponent->OnGruntHealthChanged.AddDynamic(this, &ACombatManager::HandleGruntHit);
			}
		}
	}

	//ActivateGrunts();
}

void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Increment the timer
	TimeSinceLastUpdate += DeltaTime;

	// If enough time has passed, update the grunt states
	if (TimeSinceLastUpdate >= TickInterval)
	{
		TimeSinceLastUpdate = 0.0f;
		UpdateClosestGrunts();
	}

}

void ACombatManager::HandleGruntDeath()
{
	// Identify and remove the dead grunt

	for (int32 i = AttackingGrunts.Num() - 1; i >= 0; --i)
	{
		if (AttackingGrunts[i]->HealthComp->CurrentHealth <= 0)
		{
			AttackingGrunts.RemoveAt(i);
		}

	}

	for (int32 i = StandbyGrunts.Num() - 1; i >= 0; --i)
	{
		if (StandbyGrunts[i]->HealthComp->CurrentHealth <= 0)
		{
			StandbyGrunts.RemoveAt(i);
		}
	}

	// Refill attacking slots
	OnGruntDeath.Broadcast(AttackingGrunts.Num() + StandbyGrunts.Num());
	GetWorldTimerManager().SetTimer(IdleDialogueHandle,this,&ACombatManager::FireHintDialogue,IdleTimerLength,true);
	UpdateGruntStates();

	if (!bIsComplete && AttackingGrunts.Num() <= 0 && StandbyGrunts.Num() <= 0)
	{
		if (PostCombatMusicClass)
		{
			if (UAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
			{
				AudioSubsystem->SetMusic(PostCombatMusicClass, 4.0f, 8.0f);
			}
		}
		bIsComplete = true;
		OnCombatManagerDeactivated.Broadcast();
	}
}

void ACombatManager::HandleGruntHit(AGruntShotgunEnemy* Grunt, float NewHealth)
{
	// Validate the grunt and its health
	if (!activated)
	{
		ActivateGrunts();
		if(ATriggerTarget* TriggerTarget = StartCombatEarlyTrigger.Get())
		{
			TriggerTarget->Trigger();
		}
	}
	if (!Grunt)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleGruntHit: Grunt is null."));
		return;
	}

	if (NewHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleGruntHit: Grunt %s is dead or has no health remaining."), *Grunt->GetName());
		return;
	}

	// Check if the grunt is currently in standby
	if (StandbyGrunts.Contains(Grunt))
	{
		// Promote the hit grunt to attacking
		StandbyGrunts.Remove(Grunt);
		AttackingGrunts.Add(Grunt);
		Grunt->SetState(GruntAttacking);

		UE_LOG(LogTemp, Warning, TEXT("Grunt %s hit and promoted to attacking."), *Grunt->GetName());

		// Ensure the max number of attacking grunts is not exceeded
		if (AttackingGrunts.Num() > NumberOfAttackingGrunts)
		{
			// Demote the first attacking grunt back to standby
			AGruntShotgunEnemy* DemotedGrunt = AttackingGrunts[0];
			AttackingGrunts.RemoveAt(0);
			StandbyGrunts.Add(DemotedGrunt);
			if (DemotedGrunt)
			{
				DemotedGrunt->SetState(GruntStandby);
				UE_LOG(LogTemp, Warning, TEXT("Grunt %s demoted to standby."), *DemotedGrunt->GetName());
			}
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleGruntHit: Grunt %s is not in standby or already attacking."), *Grunt->GetName());
	}
}
// UpdateGruntStates
void ACombatManager::UpdateGruntStates()
{
	if (activated)
	{
		while (AttackingGrunts.Num() < NumberOfAttackingGrunts && StandbyGrunts.Num() > 0)
		{
			AGruntShotgunEnemy* NextGrunt = StandbyGrunts.Pop();
			AttackingGrunts.Add(NextGrunt);
			if (NextGrunt)
			{
				NextGrunt->SetState(GruntAttacking);
			}
		}
		if (AttackingGrunts.Num() <= 0 && StandbyGrunts.Num() <= 0)
		{
			if (DuoQGameMode)
			{
				DuoQGameMode->UnregisterCombatManager(this);
			}

			if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
			{
				FName levelName = FName(*GetLevel()->GetOuter()->GetName());
				SaveGameInstance->EnemyStatus.Add(levelName, true);
				UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
			}

			GetWorldTimerManager().ClearTimer(IdleDialogueHandle);
		}
	}
}

void ACombatManager::ActivateGrunts()
{
	if (activated)
	{
		return;
	}
	activated = true;
	DuoQGameMode->RegisterCombatManager(this);
	OnCombatManagerActivated.Broadcast();
	// Get the player character
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerPawn)
		return;

	ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(PlayerPawn);
	if (!PlayerCharacter)
		return;
	Grunts.RemoveAll([](AGruntShotgunEnemy* Grunt) { return Grunt == nullptr; });
	// Sort grunts by distance to the player
	Grunts.Sort([this](const AActor& A, const AActor& B) {
		float DistanceA = FVector::Dist(StartOfCombat->GetComponentLocation(), A.GetActorLocation());
		float DistanceB = FVector::Dist(StartOfCombat->GetComponentLocation(), B.GetActorLocation());
		return DistanceA < DistanceB;
	});

	UAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>();
	if (AudioSubsystem)
	{
		if (CombatMusicClass)
		{
			AudioSubsystem->SetMusic(CombatMusicClass);
		}
		AudioSubsystem->OnAddEnemy();
	}

	for (AGruntShotgunEnemy* Grunt : Grunts)
	{
		if (Grunt)
		{
			if (AttackingGrunts.Num() < NumberOfAttackingGrunts)
			{
				AttackingGrunts.Add(Grunt);
				Grunt->SetState(GruntAttacking);
			}
			else
			{
				StandbyGrunts.Add(Grunt);
				Grunt->SetState(GruntStandby);
			}
		}
	}

	GetWorldTimerManager().SetTimer(IdleDialogueHandle,this,&ACombatManager::FireHintDialogue,IdleTimerLength,true);
}

void ACombatManager::SetAllGruntsToStandby()
{
	activated = false;
	DuoQGameMode->UnregisterCombatManager(this);

	StandbyGrunts.Empty();
	AttackingGrunts.Empty();

	// Update states
	for (AGruntShotgunEnemy* Grunt : Grunts)
	{
		if (Grunt)
		{
			Grunt->SetState(GruntIdle);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("All grunts set to standby."));
}


EGruntState ACombatManager::GetGruntState(AGruntShotgunEnemy* Grunt) const
{
	if (AttackingGrunts.Contains(Grunt))
	{
		return EGruntState::Attacking;
	}
	else if (StandbyGrunts.Contains(Grunt))
	{
		return EGruntState::Standby;
	}
	return EGruntState::NotFound;
}

void ACombatManager::AddGrunts(const TArray<AGruntShotgunEnemy*>& NewGrunts)
{
	for (AGruntShotgunEnemy* Grunt : NewGrunts)
	{
		if (Grunt && !StandbyGrunts.Contains(Grunt))
		{
			StandbyGrunts.Add(Grunt);
		}
	}

	UpdateGruntStates();
}

void ACombatManager::UpdateClosestGrunts()
{
	if (activated)
	{

		if (StandbyGrunts.Num() == 0 && AttackingGrunts.Num() == 0)
		{
			return; // No grunts to manage
		}

		// Get the player character
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (!PlayerPawn)
		{
			return; // Player not found
		}

		ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(PlayerPawn);
		if (!PlayerCharacter)
		{
			return; // Player character not valid
		}

		// Combine attacking and standby grunts into one array
		TArray<AGruntShotgunEnemy*> AllGrunts = StandbyGrunts;
		AllGrunts.Append(AttackingGrunts);
		AllGrunts.RemoveAll([](AGruntShotgunEnemy* Grunt) { return Grunt == nullptr; });
		// Sort grunts by distance to the player
		AllGrunts.Sort([this, PlayerCharacter](const AGruntShotgunEnemy& A, const AGruntShotgunEnemy& B) {
			float DistanceA = FVector::Dist(PlayerCharacter->GetActorLocation(), A.GetActorLocation());
			float DistanceB = FVector::Dist(PlayerCharacter->GetActorLocation(), B.GetActorLocation());
			return DistanceA < DistanceB;
		});

		// Clear previous arrays
		AttackingGrunts.Empty();
		StandbyGrunts.Empty();

		// Assign the closest grunts to attacking, others to standby
		for (int32 i = 0; i < AllGrunts.Num(); ++i)
		{
			if (i < NumberOfAttackingGrunts)
			{
				AttackingGrunts.Add(AllGrunts[i]);
			}
			else
			{
				StandbyGrunts.Add(AllGrunts[i]);
			}
		}

		// Update grunt states
		for (AGruntShotgunEnemy* Grunt : AttackingGrunts)
		{
			if (Grunt)
			{
				Grunt->SetState(GruntAttacking);
			}
		}

		for (AGruntShotgunEnemy* Grunt : StandbyGrunts)
		{
			if (Grunt)
			{
				Grunt->SetState(GruntStandby);
			}
		}
	}
}

void ACombatManager::FireHintDialogue()
{
	if(DialogueTriggerComponent && !DialogueTriggerComponent->TriggeredDialogue.IsEmpty())
	{
		DialogueTriggerComponent->TriggerDialogue();
	}
}

