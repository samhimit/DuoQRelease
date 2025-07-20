// Copyright 2024


#include "TheUltimateDialogueGrabber.h"
#include "GameSystems/Dialogue/DialogueTriggerComponent.h"
#include "GameSystems/Dialogue/PlayerPromptedComponent.h"

// Sets default values
ATheUltimateDialogueGrabber::ATheUltimateDialogueGrabber()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATheUltimateDialogueGrabber::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATheUltimateDialogueGrabber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATheUltimateDialogueGrabber::GrabDialogue()
{
	UObject* DefaultObject = BlueprintSubsystem->GeneratedClass->GetDefaultObject();

	// Change the property value
	FProperty* DollarProperty = DefaultObject->GetClass()->FindPropertyByName(TEXT("DollarTables"));
	FProperty* CalloutProperty = DefaultObject->GetClass()->FindPropertyByName(TEXT("CalloutTables"));
	if (DollarProperty && CalloutProperty)
	{
		if (DollarProperty->IsA<FMapProperty>() && CalloutProperty->IsA<FMapProperty>())
		{
			FMapProperty* DollarMapProperty = static_cast<FMapProperty*>(DollarProperty);
			void* DollarMapContainer = DollarMapProperty->ContainerPtrToValuePtr<void*>(DefaultObject);
			FScriptMapHelper DollarMapHelper(DollarMapProperty, DollarMapContainer);

			FMapProperty* CalloutMapProperty = static_cast<FMapProperty*>(CalloutProperty);
			void* CalloutMapContainer = CalloutMapProperty->ContainerPtrToValuePtr<void*>(DefaultObject);
			FScriptMapHelper CalloutMapHelper(CalloutMapProperty, CalloutMapContainer);

			
			for(TObjectIterator<UDialogueTriggerComponent> DialogueIterator; DialogueIterator; ++DialogueIterator)
			{
				if(const UDialogueTriggerComponent* DialogueTriggerComponent = *DialogueIterator)
				{
					if(DialogueTriggerComponent->TriggeredDialogue != "")
					{
						if(DialogueTriggerComponent->bIsCritical)
						{
							DollarMapHelper.FindOrAdd(&DialogueTriggerComponent->TriggeredDialogue);
						}
						else
						{
							CalloutMapHelper.FindOrAdd(&DialogueTriggerComponent->TriggeredDialogue);
						}
					}
				}
			}

			for(TObjectIterator<UPlayerPromptedComponent> PlayerPromptedIterator; PlayerPromptedIterator; ++PlayerPromptedIterator)
			{
				if(const UPlayerPromptedComponent* PlayerPromptedComponent = * PlayerPromptedIterator)
				{
					for(auto Pair : PlayerPromptedComponent->Prompts)
					{
						DollarMapHelper.FindOrAdd(&Pair.Key);
					}
				}
			}
			
		}
	}

	// Save the changes to the blueprint
	BlueprintSubsystem->Modify();
	BlueprintSubsystem->PostEditChange();
	
}

