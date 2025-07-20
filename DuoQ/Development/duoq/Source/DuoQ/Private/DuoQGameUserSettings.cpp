// Copyright 2024


#include "DuoQGameUserSettings.h"

UDuoQGameUserSettings::UDuoQGameUserSettings()
{
	bShouldRunHardwareBenchmark = true;
}

void UDuoQGameUserSettings::SetShouldRunHardwareBenchmark(bool bNewValue)
{
	bShouldRunHardwareBenchmark = bNewValue;
	SaveConfig();
	SaveSettings();
}

UDuoQGameUserSettings* UDuoQGameUserSettings::GetDuoQGameUserSettings()
{
	return Cast<UDuoQGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}