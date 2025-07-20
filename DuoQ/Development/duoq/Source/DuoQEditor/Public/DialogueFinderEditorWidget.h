// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "DialogueFinderEditorWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQEDITOR_API UDialogueFinderEditorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> FindDialogues(FString DialogueName);
};
