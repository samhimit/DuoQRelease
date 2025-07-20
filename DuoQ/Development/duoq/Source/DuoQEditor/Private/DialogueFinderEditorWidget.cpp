// Copyright 2024


#include "DialogueFinderEditorWidget.h"

#include "GameSystems/Dialogue/DialogueTriggerComponent.h"

TArray<AActor*> UDialogueFinderEditorWidget::FindDialogues(FString DialogueName)
{
	TArray<TObjectPtr<AActor>> FoundActors;
	for(TObjectIterator<UDialogueTriggerComponent> DialogueIterator; DialogueIterator; ++DialogueIterator)
	{
		if(const UDialogueTriggerComponent* DialogueTriggerComponent = *DialogueIterator)
		{
			if(DialogueTriggerComponent->TriggeredDialogue == DialogueName)
			{
				FoundActors.Emplace(DialogueTriggerComponent->GetOwner());
			}
		}
	}
	return FoundActors;
}
