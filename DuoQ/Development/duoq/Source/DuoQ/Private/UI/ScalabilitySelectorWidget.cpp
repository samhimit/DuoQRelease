// Copyright 2024


#include "UI/ScalabilitySelectorWidget.h"

#include "DuoQGameUserSettings.h"
#include "Components/ComboBoxString.h"
#include "GameFramework/GameUserSettings.h"
#include "Scalability.h"


void UScalabilitySelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ScalabilityComboBox)
	{
		if (UGameUserSettings* GameUserSettings = UDuoQGameUserSettings::GetDuoQGameUserSettings())
		{
			if (GameUserSettings->GetOverallScalabilityLevel() == -1)
			{
				if (GameUserSettings->GetTextureQuality() == 1) //Update this in case we update Low to have higher texture settings. Currently assuming Medium has 1 low has 0.
				{
					ScalabilityComboBox->SetSelectedIndex(1);
				}

				else
				{
					ScalabilityComboBox->SetSelectedIndex(0);
				}
			}
			else
			{
				ScalabilityComboBox->SetSelectedIndex(GameUserSettings->GetOverallScalabilityLevel());
			}
			SetEngineScalability(ScalabilityComboBox->GetSelectedOption());
		}
		ScalabilityComboBox->OnSelectionChanged.AddDynamic(this, &UScalabilitySelectorWidget::OnScalabilitySelectionChanged);
	}
}

void UScalabilitySelectorWidget::SetEngineScalability(FString SelectedItem)
{
	if (UGameUserSettings* GameUserSettings = UDuoQGameUserSettings::GetDuoQGameUserSettings())
	{
		if (SelectedItem == TEXT("Low"))
		{
			GameUserSettings->SetOverallScalabilityLevel(0); // Base Low

			// Custom overrides for Low
			GameUserSettings->SetGlobalIlluminationQuality(2); // High GI
			GameUserSettings->SetShadowQuality(1);			   // Medium Shadows
		}
		else if (SelectedItem == TEXT("Medium"))
		{
			GameUserSettings->SetOverallScalabilityLevel(1); // Base Medium

			// Custom overrides for Medium
			GameUserSettings->SetGlobalIlluminationQuality(2); // High GI
		}
		else if (SelectedItem == TEXT("High"))
		{
			GameUserSettings->SetOverallScalabilityLevel(2);
		}
		else if (SelectedItem == TEXT("Epic"))
		{
			GameUserSettings->SetOverallScalabilityLevel(3);
		}
		else if (SelectedItem == TEXT("Cinematic"))
		{
			GameUserSettings->SetOverallScalabilityLevel(4);
		}
	}
}

void UScalabilitySelectorWidget::OnScalabilitySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	SetEngineScalability(SelectedItem);
	if (UGameUserSettings* GameUserSettings = UDuoQGameUserSettings::GetDuoQGameUserSettings())
	{
		GameUserSettings->ApplySettings(false);
		GameUserSettings->SaveConfig(CPF_Config, *GameUserSettings->GetDefaultConfigFilename());
	}
}