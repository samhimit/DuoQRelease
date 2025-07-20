// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/DuoQGameInstance.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"

UDialogueSubsystem* UDuoQGameInstance::GetDialogueSubsystem()
{
	return GetSubsystem<UDialogueSubsystem>();
}