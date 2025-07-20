// Copyright 2024

#include "GameSystems/Dialogue/DialogueTriggerComponent.h"

#include "Components/TextRenderComponent.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "GameSystems/Dialogue/DialogueTextRenderComponent.h"

// Sets default values for this component's properties
UDialogueTriggerComponent::UDialogueTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

void UDialogueTriggerComponent::TriggerDialogue()
{
	if (TriggeredDialogue != "")
	{
		if (!bDialogueSingleTrigger || !bHasTriggered)
		{
			if (UDialogueSubsystem* Dialogue = GetOwner()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
			{
				if (bIsCritical)
				{
					if (bIsDelayed)
					{
						GetWorld()->GetTimerManager().SetTimer(DialogueTimerHandle, this, &UDialogueTriggerComponent::TriggerDialogueByTimer, DelayedSeconds, false);
					}
					else
					{
						//bHasTriggered =
						Dialogue->ActivateBundle(TriggeredDialogue);
						bHasTriggered = true;
					}
				}
				else
				{
					if (bIsDelayed)
					{
						GetWorld()->GetTimerManager().SetTimer(DialogueTimerHandle, this, &UDialogueTriggerComponent::TriggerDialogueByTimer, DelayedSeconds, false);
					}
					else
					{
						//bHasTriggered =
						Dialogue->DoCallout(TriggeredDialogue);
						bHasTriggered = true;
					}
				}
			}
		}
	}
}

void UDialogueTriggerComponent::TriggerDialogueByTimer()
{
	if (UDialogueSubsystem* Dialogue = GetOwner()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		if (bIsCritical)
		{
			//bHasTriggered =
			Dialogue->ActivateBundle(TriggeredDialogue);
			bHasTriggered = true;
		}
		else
		{
			//bHasTriggered =
			Dialogue->DoCallout(TriggeredDialogue);
			bHasTriggered = true;
		}
	}
}

#if WITH_EDITOR
void UDialogueTriggerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (UDialogueTextRenderComponent* DialogueRenderComponent = GetOwner()->GetComponentByClass<UDialogueTextRenderComponent>())
	{
		DialogueRenderComponent->SetText(FText::FromString(TriggeredDialogue));
	}
}
#endif
