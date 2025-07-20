// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "DialogueTextRenderComponent.generated.h"

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class DUOQ_API UDialogueTextRenderComponent : public UTextRenderComponent
{
	GENERATED_BODY()

	UDialogueTextRenderComponent();

public:
	virtual void OnComponentCreated() override;
};
