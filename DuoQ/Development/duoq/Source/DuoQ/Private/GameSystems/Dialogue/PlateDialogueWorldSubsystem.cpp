// Copyright 2024


#include "GameSystems/Dialogue/PlateDialogueWorldSubsystem.h"

#include "GameSystems/DuoQGameMode.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"

bool UPlateDialogueWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UPlateDialogueWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UPlateDialogueWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPlateDialogueWorldSubsystem::AddInstruction(EPressurePlateInstructions NewInstruction)
{
	InstructionBuffer.Emplace(NewInstruction);
	CheckForDialogue();
}

void UPlateDialogueWorldSubsystem::CheckForDialogue()
{
	for(FPlateSequenceConversation SequenceConversation : Conversations)
	{
		bool SequenceMatch = true;
		int SequenceLength = SequenceConversation.InstructionSequence.Num();
		if(SequenceLength == 0)
		{
			break;
		}
		
		for(int i = 0; i < SequenceLength; i++)
		{
			if(!InstructionBuffer.IsValidIndex(i+InstructionBuffer.Num()-SequenceLength) || InstructionBuffer[i+InstructionBuffer.Num()-SequenceLength] != SequenceConversation.InstructionSequence[i] || (InstructionBuffer[i+InstructionBuffer.Num()-SequenceLength] != EPressurePlateInstructions::StepOff && SequenceConversation.InstructionSequence[i] == EPressurePlateInstructions::Any))
			{
				SequenceMatch = false;
				break;
			}
		}
		if(SequenceMatch)
		{
			if(UDialogueSubsystem* DialogueSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
			{
				DialogueSubsystem->DoCallout(SequenceConversation.ConversationName);
				return;
			}
		}
		
	}
}