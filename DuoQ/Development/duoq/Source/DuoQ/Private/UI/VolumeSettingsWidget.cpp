// Copyright 2024


#include "UI/VolumeSettingsWidget.h"

#include "AudioMixerDevice.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Components/Slider.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"
#include "GameSystems/Audio/AudioSubsystem.h"
#include "UI/SliderFillWidget.h"

void UVolumeSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	

	if (MasterVolume && ChatVolume && MusicVolume && SFXVolume)
	{
		if (UDuoQSaveSystem* SaveSystem = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
		{
			MasterVolume->Slider->SetValue(SaveSystem->MasterVolume);
			MusicVolume->Slider->SetValue(SaveSystem->MusicVolume);
			ChatVolume->Slider->SetValue(SaveSystem->ChatVolume);
			SFXVolume->Slider->SetValue(SaveSystem->SFXVolume);
		}
		
		MasterVolume->Slider->OnValueChanged.AddDynamic(this, &UVolumeSettingsWidget::OnMasterValueChanged);
		MusicVolume->Slider->OnValueChanged.AddDynamic(this, &UVolumeSettingsWidget::OnMusicValueChanged);
		ChatVolume->Slider->OnValueChanged.AddDynamic(this, &UVolumeSettingsWidget::OnChatValueChanged);
		SFXVolume->Slider->OnValueChanged.AddDynamic(this, &UVolumeSettingsWidget::OnSFXValueChanged);
	}

	
}

void UVolumeSettingsWidget::OnMasterValueChanged(float Value)
{
	if (UDuoQSaveSystem* SaveSystem = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveSystem->MasterVolume = Value;
		if(USaveSubSystem* SS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubSystem>())
		{
			SS->SaveMenuSettings(MasterVolume->Slider->GetValue(), MusicVolume->Slider->GetValue(),
				ChatVolume->Slider->GetValue(), SFXVolume->Slider->GetValue());
		}
	}
}

void UVolumeSettingsWidget::OnMusicValueChanged(float Value)
{
	if (UDuoQSaveSystem* SaveSystem = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveSystem->MusicVolume = Value;
		if(USaveSubSystem* SS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubSystem>())
		{
			SS->SaveMenuSettings(MasterVolume->Slider->GetValue(), MusicVolume->Slider->GetValue(),
				ChatVolume->Slider->GetValue(), SFXVolume->Slider->GetValue());
		}
	}
}

void UVolumeSettingsWidget::OnChatValueChanged(float Value)
{
	if (UDuoQSaveSystem* SaveSystem = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveSystem->ChatVolume = Value;
		if(USaveSubSystem* SS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubSystem>())
		{
			SS->SaveMenuSettings(MasterVolume->Slider->GetValue(), MusicVolume->Slider->GetValue(),
				ChatVolume->Slider->GetValue(), SFXVolume->Slider->GetValue());
		}
	}
}

void UVolumeSettingsWidget::OnSFXValueChanged(float Value)
{
	if (UDuoQSaveSystem* SaveSystem = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		SaveSystem->SFXVolume = Value;
		if(USaveSubSystem* SS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubSystem>())
		{
			SS->SaveMenuSettings(MasterVolume->Slider->GetValue(), MusicVolume->Slider->GetValue(),
				ChatVolume->Slider->GetValue(), SFXVolume->Slider->GetValue());
		}
	}
}
