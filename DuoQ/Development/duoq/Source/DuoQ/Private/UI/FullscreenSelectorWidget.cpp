// Copyright 2024


#include "UI/FullscreenSelectorWidget.h"

#include "DuoQGameUserSettings.h"
#include "Components/ComboBoxString.h"


void UFullscreenSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (FullscreenComboBox)
	{
		FullscreenComboBox->OnSelectionChanged.AddDynamic(this, &UFullscreenSelectorWidget::OnFullscreenSelectionChanged);
		if (UDuoQGameUserSettings* GameUserSettings = UDuoQGameUserSettings::GetDuoQGameUserSettings())
		{
			FullscreenComboBox->SetSelectedIndex(GameUserSettings->GetFullscreenMode());
		}
	}
}

void UFullscreenSelectorWidget::OnFullscreenSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (UDuoQGameUserSettings* GameUserSettings = UDuoQGameUserSettings::GetDuoQGameUserSettings())
	{
		
		if (SelectedItem == TEXT("Fullscreen"))
		{
			GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		}
		else if (SelectedItem == TEXT("Windowed Fullscreen"))
		{
			GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		}
		else if (SelectedItem == TEXT("Windowed"))
		{
			GameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
		}
		
		GameUserSettings->ApplySettings(false);
	}
}
