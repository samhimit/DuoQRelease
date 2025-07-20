// Copyright 2024


#include "GameSystems/Dialogue/PlayerPromptedComponent.h"

#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "GameSystems/Progression/TriggerTarget.h"

// Sets default values for this component's properties
UPlayerPromptedComponent::UPlayerPromptedComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UPlayerPromptedComponent::BeginPlay()
{
	Super::BeginPlay();

	if(ATriggerTarget* TriggerTarget = Cast<ATriggerTarget>(GetOwner()))
	{
		TriggerTarget->OnTriggered.AddDynamic(this,&UPlayerPromptedComponent::UpdatePrompts);
	}
	
}

void UPlayerPromptedComponent::UpdatePrompts()
{
	if(UDialogueSubsystem* DialogueSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		for(auto Pair : Prompts)
		{
			if(bAddPrompts)
			{
				DialogueSubsystem->ConvoStarts.FindOrAdd(Pair.Key,Pair.Value);
			}
			else
			{
				DialogueSubsystem->ConvoStarts.Remove(Pair.Key);
			}
			
		}
	}
}

