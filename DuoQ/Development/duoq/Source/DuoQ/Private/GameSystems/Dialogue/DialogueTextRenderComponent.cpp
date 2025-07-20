// Copyright 2024


#include "GameSystems/Dialogue/DialogueTextRenderComponent.h"

#include "GameSystems/Dialogue/DialogueTriggerComponent.h"

UDialogueTextRenderComponent::UDialogueTextRenderComponent()
{
	//SetText(FText::FromString(DialogueTriggerComponent->TriggeredDialogue));
	SetTextRenderColor(FColor::Red);
	SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	bIsEditorOnly = true;
	bHiddenInGame = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RetrievedMaterial(TEXT("/Script/Engine.Material'/Game/GameSystems/Dialogue/DuoQDialogueTextMaterial.DuoQDialogueTextMaterial'"));
	UMaterialInterface*											 Material = RetrievedMaterial.Object;
	// Set the sprite texture
	if (Material)
	{
		SetTextMaterial(Material);
	}
	SetUsingAbsoluteRotation(true);
	SetUsingAbsoluteScale(true);
}

void UDialogueTextRenderComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if(UDialogueTriggerComponent* DialogueTriggerComponent = GetOwner()->GetComponentByClass<UDialogueTriggerComponent>())
	{
		SetText(FText::FromString(DialogueTriggerComponent->TriggeredDialogue));
	}
}
