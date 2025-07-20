// Copyright 2024

#include "Characters/Partner/MoveToCoverAction.h"
#include "Engine/World.h"


void UMoveToCoverAction::ResetTimer()
{
	NumTimeWarned += 1;
	bShouldSetTimer = true;
	bShouldWarn = true;
}
