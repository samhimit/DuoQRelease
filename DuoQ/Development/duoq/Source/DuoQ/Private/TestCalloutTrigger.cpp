// Copyright 2024


#include "TestCalloutTrigger.h"

#include "Characters/Player/DuoQPlayerCharacter.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"

void ATestCalloutTrigger::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ATestCalloutTrigger::OnPlayerEnterTriggerBox);
}

void ATestCalloutTrigger::OnPlayerEnterTriggerBox(AActor* OverlappedActor, AActor* OtherActor)
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
			DialogueSubsystem->DoCallout(CalloutTableName);

			// Optionally, log that the dialogue has been triggered
			UE_LOG(LogTemp, Log, TEXT("Callout triggered with CalloutTableID: %s"), *CalloutTableName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DialogueSubsystem not found in the GameInstance!"));
		}
	}
}