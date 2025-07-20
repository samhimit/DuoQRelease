// Copyright 2024


#include "UI/MouseSensitivityWidget.h"

#include <string>

#include "DuoQBlueprintFunctionLibrary.h"
#include "DuoQGameUserSettings.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "UI/SliderFillWidget.h"


void UMouseSensitivityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Controller = UDuoQBlueprintFunctionLibrary::GetDuoQPlayerController(GetWorld(), 0);

	//Need to get player input somehow on main menu... controller doesn't exist there
	
	if (SensitivitySlider && Controller)
	{
		SensitivitySlider->ProgressBar->SetPercent(MouseSensitivity);
		MouseSensitivity *= MouseSensitivityMultiplier;
		SensitivitySlider->Slider->OnValueChanged.AddDynamic(this, &UMouseSensitivityWidget::OnSensitivityChanged);
		Controller->AddPitchInput(MouseSensitivity);
		Controller->AddYawInput(MouseSensitivity);
		if(UPlayerInput* PlayerInput = Controller->PlayerInput)
		{
			PlayerInput->LoadConfig();
			MouseSensitivity = PlayerInput->GetMouseSensitivityX();
			PlayerInput->SetMouseSensitivity(PlayerInput->GetMouseSensitivityX(), PlayerInput->GetMouseSensitivityY());
			SensitivitySlider->Slider->SetValue(MouseSensitivity);
		}
	}
}

void UMouseSensitivityWidget::OnSensitivityChanged(float Value)
{
	if (Controller)
	{
		MouseSensitivity = Value * MouseSensitivityMultiplier;
		Controller->AddPitchInput(MouseSensitivity);
		Controller->AddYawInput(MouseSensitivity);
		Text->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(Value * 100))));
		if(UPlayerInput* PlayerInput = Controller->PlayerInput)
		{
			if (UDuoQGameUserSettings* GameUserSettings = UDuoQGameUserSettings::GetDuoQGameUserSettings())
			{
				PlayerInput->SetMouseSensitivity(MouseSensitivity);
				PlayerInput->SaveConfig();
			}
		}
	}
}
