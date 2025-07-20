// Copyright 2024


#include "GameSystems/Audio/KeyboardAudioSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UKeyboardAudioSubsystem::PlayWASDPressSound()
{
	// Only play sound if a certain number of sounds havn't been attempted
	// to be played recently (to prevent spamming)
	if (ConsecutiveActions < MaxConsecutiveActions)
	{
		ConsecutiveActions++;
		UGameplayStatics::PlaySound2D(this, WASDPress);
	}

	GetWorld()->GetTimerManager().SetTimer(ActionCooldownTimer, this, &UKeyboardAudioSubsystem::ResetConsecutiveActions, Cooldown, false);
}

void UKeyboardAudioSubsystem::PlayWASDReleaseSound()
{
	if (ConsecutiveActions < MaxConsecutiveActions)
	{
		ConsecutiveActions++;
		UGameplayStatics::PlaySound2D(this, WASDPress);
	}

	GetWorld()->GetTimerManager().SetTimer(ActionCooldownTimer, this, &UKeyboardAudioSubsystem::ResetConsecutiveActions, Cooldown, false);
}

void UKeyboardAudioSubsystem::ResetConsecutiveActions()
{
	ConsecutiveActions = 0;
}
