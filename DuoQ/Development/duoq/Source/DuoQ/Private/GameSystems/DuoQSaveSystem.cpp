// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/DuoQGameInstance.h"

UDuoQSaveSystem::UDuoQSaveSystem()
{
	SaveSlotName = TEXT("TestSaveSlot");
	PlayerProgress = -1;
	#if WITH_EDITOR
	PlayerProgress = 0;
	#endif
	UserIndex = 0;
}