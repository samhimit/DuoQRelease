// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueTriggerComponent.generated.h"

class UTextRenderComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class DUOQ_API UDialogueTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDialogueTriggerComponent();

public:
	UPROPERTY(EditAnywhere, Category = "Dialogue", BlueprintReadWrite)
	FString TriggeredDialogue = "";

	UPROPERTY(EditAnywhere, Category = "Dialogue", BlueprintReadWrite)
	bool bIsCritical = true;
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool bDialogueSingleTrigger = true;
	UPROPERTY(EditAnywhere, Category = "Dialogue", BlueprintReadWrite)
	bool bIsDelayed = false;
	UPROPERTY(EditAnywhere, Category = "Dialogue", BlueprintReadWrite)
	float DelayedSeconds = 0.0f;

protected:
	UPROPERTY()
	bool bHasTriggered = false;

	UFUNCTION()
	void TriggerDialogueByTimer();

	FTimerHandle DialogueTimerHandle;

public:
	UFUNCTION(BlueprintCallable)
	virtual void TriggerDialogue();
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
