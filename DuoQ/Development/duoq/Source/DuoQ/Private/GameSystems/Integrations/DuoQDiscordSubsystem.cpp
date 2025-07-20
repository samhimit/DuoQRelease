// Copyright 2024


#include "GameSystems/Integrations/DuoQDiscordSubsystem.h"

UDuoQDiscordSubsystem::UDuoQDiscordSubsystem()
{
	ClientId = 1363953803019157625;

	// Configure these as desired for your game:
	MinimumLogLevel = discord::LogLevel::Debug;
	CreateRetryTime = 5.0f;
}

bool UDuoQDiscordSubsystem::UpdateActivity()
{
	bool bResult {false};

	if (IsDiscordRunning())
	{
		bResult = true;

		discord::Activity Activity {};
		Activity.SetType(discord::ActivityType::Playing);
		Activity.SetApplicationId(ClientId);
		Activity.SetName(TCHAR_TO_ANSI(*GameName));  // (Note: This value does not seem to be used)
		
		Activity.SetState(TCHAR_TO_ANSI(*LargeText));
		Activity.SetDetails(TCHAR_TO_ANSI(*LobbyText));
		Activity.SetType(discord::ActivityType::Playing);
		
		Activity.SetSupportedPlatforms(static_cast<uint32_t>(discord::ActivitySupportedPlatformFlags::Desktop));

		discord::ActivityTimestamps& Timestamps = Activity.GetTimestamps();
		Timestamps.SetStart(StartTime);

		discord::ActivityAssets& Assets = Activity.GetAssets();
		Assets.SetLargeImage(TCHAR_TO_ANSI(*LargeImageName));
		
		Assets.SetSmallImage(TCHAR_TO_ANSI(*SmallImageName));
		Assets.SetSmallText(TCHAR_TO_ANSI(*SmallImageText));

		DiscordCore().ActivityManager().UpdateActivity(Activity, [this](discord::Result Result)
		{
			const FString RequestDescription (TEXT("Updating Activity"));
			LogDiscordResult(Result, RequestDescription);
		});
	}
	return bResult;
}

bool UDuoQDiscordSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	#if PLATFORM_MAC
		return false;
	#endif

	return Super::ShouldCreateSubsystem(Outer);
}

void UDuoQDiscordSubsystem::NativeOnDiscordCoreCreated()
{
	Super::NativeOnDiscordCoreCreated();
	StartTime = FDateTime::UtcNow().ToUnixTimestamp();
	UpdateActivity();
}

void UDuoQDiscordSubsystem::NativeOnDiscordCoreReset()
{
	Super::NativeOnDiscordCoreReset();
}

bool UDuoQDiscordSubsystem::SetLobbyText(FString Big, FString Small)
{
	LobbyText = Big;
	LargeText= Small;
	return UpdateActivity();
}