// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PlateDialogueWorldSubsystem.generated.h"

UENUM(BlueprintType)
enum class EPressurePlateInstructions : uint8
{
	StepOff,
	Pink,
	Black,
	Orange,
	Any
};

USTRUCT(BlueprintType)
struct FPlateSequenceConversation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	FString ConversationName = "";
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<EPressurePlateInstructions> InstructionSequence;
};


/**
 * 
 */
UCLASS(Blueprintable,Abstract)
class DUOQ_API UPlateDialogueWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable)
	void AddInstruction(EPressurePlateInstructions NewInstruction);

	

protected:
	void CheckForDialogue();
	
	UPROPERTY()
	TArray<EPressurePlateInstructions> InstructionBuffer;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	TArray<FPlateSequenceConversation> Conversations;
};
