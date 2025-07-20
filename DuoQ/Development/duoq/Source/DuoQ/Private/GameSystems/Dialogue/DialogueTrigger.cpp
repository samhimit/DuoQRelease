#include "GameSystems/Dialogue/DialogueTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"

ADialogueTrigger::ADialogueTrigger()
{
}

void ADialogueTrigger::BeginPlay()
{
	Super::BeginPlay();

	// Bind the overlap event
	OnActorBeginOverlap.AddDynamic(this, &ADialogueTrigger::OnPlayerEnterTriggerBox);
}

void ADialogueTrigger::OnPlayerEnterTriggerBox(AActor* OverlappedActor, AActor* OtherActor)
{
	// Check if the other actor is the player character
	ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		// Access the DialogueSubsystem from the GameInstance
		UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();

		if (DialogueSubsystem)
		{
			// Call the ActivateBundle function in the DialogueSubsystem
			DialogueSubsystem->ActivateBundle(BundleName);

			// Optionally, log that the dialogue has been triggered
			UE_LOG(LogTemp, Log, TEXT("Dialogue triggered with BundleID: %s"), *BundleName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DialogueSubsystem not found in the GameInstance!"));
		}
	}
}
