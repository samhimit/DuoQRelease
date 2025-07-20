// Copyright 2024


#include "Characters/Enemy/Amicus.h"
#include "Characters/Enemy/AmicusWeakPoint.h"
#include "Misc/OutputDeviceNull.h"
#include "Kismet/GameplayStatics.h"
AAmicus::AAmicus()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxTimeTillDeath = 120.0f;
	CorrectWeakPointsDestroyed = 0;

}

void AAmicus::BeginPlay()
{
	Super::BeginPlay();

	FindAllWeakPoints();
	SelectCorrectWeakPoints();
	FindPlayerAndStarlight();
	StartTimer();
	
}

void AAmicus::FindAllWeakPoints()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAmicusWeakPoint::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (AAmicusWeakPoint* WeakPoint = Cast<AAmicusWeakPoint>(Actor))
		{
			// Skip weak points that are children of other actors
			if (WeakPoint->GetAttachParentActor() != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Skipped weak point: %s (attached to %s)"),
					*WeakPoint->GetName(),
					*WeakPoint->GetAttachParentActor()->GetName());
				continue;
			}

			// Add the weak point to the list
			WeakPoints.Add(WeakPoint);

			// Bind the delegate to handle weak point destruction
			WeakPoint->OnWeakPointDestroyed.AddDynamic(this, &AAmicus::OnWeakPointDestroyed);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Found %d weak points."), WeakPoints.Num());
}

void AAmicus::SelectCorrectWeakPoints()
{
	if (WeakPoints.Num() < 3)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough weak points to select."));
		return;
	}

	TArray<int32> SelectedIndices;
	while (SelectedIndices.Num() < 3)
	{
		int32 RandomIndex = FMath::RandRange(0, WeakPoints.Num() - 1);
		if (!SelectedIndices.Contains(RandomIndex))
		{
			SelectedIndices.Add(RandomIndex);
			CorrectWeakPoints.Add(WeakPoints[RandomIndex]);
			CorrectWeakPointColors.Add(WeakPoints[RandomIndex]->WeakPointColor);
		}
	}

	for (const FLinearColor& Color : CorrectWeakPointColors)
	{
		UE_LOG(LogTemp, Warning, TEXT("Correct Weak Point Color: %s"), *Color.ToString());
	}
}

void AAmicus::OnWeakPointDestroyed(FLinearColor DestroyedWeakPointColor)
{
	if (CorrectWeakPointColors.Contains(DestroyedWeakPointColor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Correct weak point destroyed: %s"), *DestroyedWeakPointColor.ToString());
		OnCorrectWeakPointDestroyed();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Incorrect weak point destroyed: %s"), *DestroyedWeakPointColor.ToString());
		OnWrongWeakPointDestroyed();
	}
}

void AAmicus::FindPlayerAndStarlight()
{
	// Find DuoQPlayerCharacter
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADuoQPlayerCharacter::StaticClass(), FoundPlayers);
	if (FoundPlayers.Num() > 0)
	{
		PlayerRef = Cast<ADuoQPlayerCharacter>(FoundPlayers[0]);
		UE_LOG(LogTemp, Warning, TEXT("Player reference saved: %s"), *PlayerRef->GetName());
	}

	// Find StarlightCharacter
	TArray<AActor*> FoundStarlights;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStarlightCharacter::StaticClass(), FoundStarlights);
	if (FoundStarlights.Num() > 0)
	{
		StarlightRef = Cast<AStarlightCharacter>(FoundStarlights[0]);
		UE_LOG(LogTemp, Warning, TEXT("Starlight reference saved: %s"), *StarlightRef->GetName());
	}

	// Ensure both references are valid
	if (!PlayerRef || !StarlightRef)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find DuoQPlayer or Starlight in the level!"));
	}
}

void AAmicus::StartTimer()
{
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AAmicus::TimerExpired, MaxTimeTillDeath, false);
}

void AAmicus::TimerExpired()
{
	UE_LOG(LogTemp, Warning, TEXT("Timer expired!"));

	// Apply 1000 damage to both Player and Starlight
	if (PlayerRef)
	{
		UHealthComponent* PlayerHealth = PlayerRef->FindComponentByClass<UHealthComponent>();
		if (PlayerHealth)
		{
			PlayerHealth->HandleDamage(1000);
		}
	}

	if (StarlightRef)
	{
		UHealthComponent* StarlightHealth = StarlightRef->FindComponentByClass<UHealthComponent>();
		if (StarlightHealth)
		{
			StarlightHealth->HandleDamage(1000);
		}
	}
}

void AAmicus::OnCorrectWeakPointDestroyed()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	StartTimer();

	CorrectWeakPointsDestroyed++;

	// Check if all correct weak points are destroyed
	if (CorrectWeakPointsDestroyed >= 3)
	{
		EncounterNumber++;

		if (EncounterNumber >= 3)
		{
			UE_LOG(LogTemp, Warning, TEXT("All correct weak points destroyed. Timer canceled."));
			GetWorldTimerManager().ClearTimer(TimerHandle); // Cancel the timer
		}

		else
		{
			UnlitWeakPoints();
			CorrectWeakPoints.Empty();
			CorrectWeakPointColors.Empty();
			SelectCorrectWeakPoints();
			StartTimer();
		}
		
	}
}

void AAmicus::OnWrongWeakPointDestroyed()
{

	// Apply 1000 damage to both Player and Starlight
	if (PlayerRef)
	{
		UHealthComponent* PlayerHealth = PlayerRef->FindComponentByClass<UHealthComponent>();
		if (PlayerHealth)
		{
			PlayerHealth->HandleDamage(1000);
		}
	}

	if (StarlightRef)
	{
		UHealthComponent* StarlightHealth = StarlightRef->FindComponentByClass<UHealthComponent>();
		if (StarlightHealth)
		{
			StarlightHealth->HandleDamage(1000);
		}
	}
}

void AAmicus::LightUpWeakPoint()
{
	if (CorrectWeakPoints.Num() > 0)
	{
		// Iterate through the weak points in the array
		for (AAmicusWeakPoint* WeakPointToLightUp : CorrectWeakPoints)
		{
			if (WeakPointToLightUp)
			{
				// Get attached actors for the current weak point
				TArray<AActor*> AttachedActors;
				WeakPointToLightUp->GetAttachedActors(AttachedActors);

				for (AActor* AttachedActor : AttachedActors)
				{
					if (AAmicusWeakPoint* ChildWeakPoint = Cast<AAmicusWeakPoint>(AttachedActor))
					{
						// Call the CorrectWeakSpot function on the child weak point
						FName FunctionName = TEXT("CorrectWeakSpot");
						if (ChildWeakPoint->FindFunction(FunctionName))
						{
							FOutputDeviceNull OutputDevice; // Create an output device
							ChildWeakPoint->CallFunctionByNameWithArguments(*FunctionName.ToString(), OutputDevice, nullptr, true);
							UE_LOG(LogTemp, Warning, TEXT("Child weak point lit up: %s"), *ChildWeakPoint->GetName());
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("CorrectWeakSpot function not found on child: %s"), *ChildWeakPoint->GetName());
						}
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Null weak point encountered in CorrectWeakPoints array."));
			}
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No weak points available to process."));
	}
}

void AAmicus::UnlitWeakPoints()
{
	if (CorrectWeakPoints.Num() > 0)
	{
		// Iterate through all correct weak points
		for (AAmicusWeakPoint* WeakPointToReset : CorrectWeakPoints)
		{
			if (WeakPointToReset)
			{
				// Get attached actors for the current weak point
				TArray<AActor*> AttachedActors;
				WeakPointToReset->GetAttachedActors(AttachedActors);

				for (AActor* AttachedActor : AttachedActors)
				{
					if (AAmicusWeakPoint* ChildWeakPoint = Cast<AAmicusWeakPoint>(AttachedActor))
					{
						// Call the ResetEmission function on the child weak point
						FName FunctionName = TEXT("ResetEmission");
						if (ChildWeakPoint->FindFunction(FunctionName))
						{
							FOutputDeviceNull OutputDevice; // Create an output device
							ChildWeakPoint->CallFunctionByNameWithArguments(*FunctionName.ToString(), OutputDevice, nullptr, true);
							UE_LOG(LogTemp, Warning, TEXT("Child weak point emission reset: %s"), *ChildWeakPoint->GetName());
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("ResetEmission function not found on child: %s"), *ChildWeakPoint->GetName());
						}
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Null weak point encountered in CorrectWeakPoints array."));
			}
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No weak points available to reset emission."));
	}
}







