// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/Audio/AudioSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystems/Audio/MusicManager.h"
#include "AudioDevice.h"
#include "AudioModulationStatics.h"
#include "Characters/Enemy/DuoQEnemyCharacter.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"

void UAudioSubsystem::Deinitialize()
{
	if (ActiveMusicPlayer)
	{
		ActiveMusicPlayer->Stop();
		ActiveMusicPlayer = nullptr;
	}
	if (ReadyMusicPlayer)
	{
		ReadyMusicPlayer->Stop();
		ReadyMusicPlayer = nullptr;
	}
	if (EndingMusicPlayer)
	{
		EndingMusicPlayer->Stop();
		EndingMusicPlayer = nullptr;
	}
	
	Super::Deinitialize();
}

void UAudioSubsystem::LoadCheckpointMusic(int PlayerProgress)
{
	if (!bEventBound)
	{
		if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
		{
			DialogueSubsystem->OnEventRaised.AddDynamic(this, &UAudioSubsystem::EventTriggered);
			DialogueSubsystem->OnDialogueEnd.AddDynamic(this, &UAudioSubsystem::OnDialogueEnd);
		}
		bEventBound = true;
	}

	StopMusic();
	for (int i = CheckpointMusic.Num() - 1; i >= 0; i--)
	{
		if (PlayerProgress >= CheckpointMusic[i].CheckpointNumber)
		{
			SetMusic(CheckpointMusic[i].MusicManagerClass, 2.0f, 2.0f, false, true);
			
			return;
		}
	}
	SetMusic(CheckpointMusic[0].MusicManagerClass, 2.0f, 2.0f, false, true);
}

void UAudioSubsystem::TransitionToNext(float FadeOutTime, float FadeInTime)
{
	if (ReadyMusicPlayer)
	{
		if (ActiveMusicPlayer)
		{
			StopMusic(FadeOutTime);
		}
		ActiveMusicPlayer = ReadyMusicPlayer;
		ReadyMusicPlayer = nullptr;
		ActiveMusicPlayer->OnCreate(FadeInTime);
		if (ActiveMusicPlayer->bIsAmbient)
		{
			ApplyConversationMix();
		}
		else
		{
			ApplyCombatMix();
		}
	}
}

void UAudioSubsystem::OnGameOver()
{
	if (ActiveMusicPlayer)
	{
		ActiveMusicPlayer->Reset();
	}
}

void UAudioSubsystem::SetMusic(TSubclassOf<UMusicManager> MusicManagerClass, float FadeOutTime, float FadeInTime, bool bPersist, bool bPlayImmediately)
{
	if (bBlockNewMusic)
	{
		return;
	}

	if (ActiveMusicPlayer && ActiveMusicPlayer.GetClass() == MusicManagerClass)
	{
		return;
	}

	if (ActiveMusicPlayer)
	{
		if (!bPlayImmediately)
		{
			if (ActiveMusicPlayer->bUsesTransition)
			{
				// If the current music has a custom transition segment, wait until it's done before transitioning
				ReadyMusicPlayer = NewObject<UMusicManager>(this, (UClass*)MusicManagerClass);
				if (bPersist)
				{
					ReadyMusicPlayer->bIgnoreForFlushing = true;
				}
				ReadyMusicPlayer->AudioDeviceID = GetWorld()->GetAudioDeviceRaw()->DeviceID;
				OnTransitionStartEvent.Broadcast(TEXT("PlayStinger"));
				return;
			}
			else if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
			{
				// If Starlight is talking, wait until the dialogue is done before transitioning
				if (DialogueSubsystem->GetIsStarlightTalking())
				{
					ReadyMusicPlayer = NewObject<UMusicManager>(this, (UClass*)MusicManagerClass);
					if (bPersist)
					{
						ReadyMusicPlayer->bIgnoreForFlushing = true;
					}
					ReadyMusicPlayer->AudioDeviceID = GetWorld()->GetAudioDeviceRaw()->DeviceID;
					return;
				}
			}
		}
		
		StopMusic(FadeOutTime);
	}

	ActiveMusicPlayer = NewObject<UMusicManager>(this, (UClass*)MusicManagerClass);
	if (bPersist)
	{
		ActiveMusicPlayer->bIgnoreForFlushing = true;
	}
	ActiveMusicPlayer->AudioDeviceID = GetWorld()->GetAudioDeviceRaw()->DeviceID;
	ActiveMusicPlayer->OnCreate(FadeInTime);
	if (ActiveMusicPlayer->bIsAmbient)
	{
		ApplyConversationMix();
	}
	else
	{
		ApplyCombatMix();
	}
}

void UAudioSubsystem::StopMusic(float FadeTime)
{
	if (ActiveMusicPlayer)
	{
		EndingMusicPlayer = ActiveMusicPlayer;
		ActiveMusicPlayer = nullptr;
		EndingMusicPlayer->OnStop(FadeTime);
	}
}

void UAudioSubsystem::OnAddEnemy()
{
	if (bAddLayerOnNextEnemy)
	{
		bAddLayerOnNextEnemy = false;
		if (ActiveMusicPlayer)
		{
			ActiveMusicPlayer->AddLayer();
		}
	}
}

void UAudioSubsystem::EventTriggered(FString EventName)
{
	if (EventMusicMap.Contains(EventName))
	{
		
		if (EventName.Equals(TEXT("EndGameEarly")))
		{
			bBlockNewMusic = true;
			SetMusic(EventMusicMap[EventName], 4.0f, 4.0f, false, true);
		}
		else
		{
			SetMusic(EventMusicMap[EventName]);
		}
	}
	else if (EventName.Equals(ComplimentStartEventName))
	{
		if (ActiveMusicPlayer)
		{
			ActiveMusicPlayer->AddLayer();
		}
	}
}

void UAudioSubsystem::OnDialogueEnd(FString BundleName)
{
	if (ReadyMusicPlayer)
	{
		if (ActiveMusicPlayer)
		{
			if (!ActiveMusicPlayer->bUsesTransition)
			{
				// Music with transitions don't need to wait for dialogue to finish since the transitions are long anyways
				TransitionToNext();
			}
		}
		else
		{
			TransitionToNext();
		}
	}
}

void UAudioSubsystem::PlayPersistentSFX(USoundBase* Sound)
{
	PersistentSFXPlayer = UGameplayStatics::CreateSound2D(this, Sound, 1.0f, 1.0f, 0.0f, nullptr, true);
	PersistentSFXPlayer->Play();
}

void UAudioSubsystem::SetMasterVolume(float Volume)
{
	UGameplayStatics::SetSoundMixClassOverride(this, MasterMix, MasterClass, Volume, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, MasterMix);
}

void UAudioSubsystem::SetMusicVolume(float Volume)
{
	UGameplayStatics::SetSoundMixClassOverride(this, MusicMix, MusicClass, Volume, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, MusicMix);
}

void UAudioSubsystem::SetSfxVolume(float Volume)
{
	UGameplayStatics::SetSoundMixClassOverride(this, SfxMix, SfxClass, Volume, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, SfxMix);
}

void UAudioSubsystem::SetVoiceVolume(float Volume)
{
	UGameplayStatics::SetSoundMixClassOverride(this, VoiceMix, VoiceClass, Volume, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, VoiceMix);
}

void UAudioSubsystem::InputDuckVolume()
{
	UGameplayStatics::SetSoundMixClassOverride(this, InputDuckingMix, InputDuckingClass, InputDuckingModifer, 1.0f, InputDuckingFadeTime);
	UGameplayStatics::PushSoundMixModifier(this, InputDuckingMix);
}

void UAudioSubsystem::InputUnduckVolume()
{
	UGameplayStatics::SetSoundMixClassOverride(this, InputDuckingMix, InputDuckingClass, 1.0f, 1.0f, InputDuckingFadeTime);
	UGameplayStatics::PushSoundMixModifier(this, InputDuckingMix);
}

void UAudioSubsystem::ApplyConversationMix()
{
	/*
	if (!UAudioModulationStatics::IsControlBusMixActive(this, ConversationMix))
	{
		UAudioModulationStatics::DeactivateBusMix(this, CombatMix);
		UAudioModulationStatics::ActivateBusMix(this, ConversationMix);
	}
	*/
}

void UAudioSubsystem::ApplyCombatMix()
{
	/*
	if (!UAudioModulationStatics::IsControlBusMixActive(this, CombatMix))
	{
		UAudioModulationStatics::DeactivateBusMix(this, ConversationMix);
		UAudioModulationStatics::ActivateBusMix(this, CombatMix);
	}
	*/
}

void UAudioSubsystem::ApplyPauseMix()
{
	UAudioModulationStatics::ActivateBusMix(this, PauseMix);
}

void UAudioSubsystem::RemovePauseMix()
{
	UAudioModulationStatics::DeactivateBusMix(this, PauseMix);
}
