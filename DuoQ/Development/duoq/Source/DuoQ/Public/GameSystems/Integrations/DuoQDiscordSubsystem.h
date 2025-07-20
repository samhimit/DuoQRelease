// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "DiscordGame/DiscordGameSubsystem.h"
#include "DuoQDiscordSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UDuoQDiscordSubsystem : public UDiscordGameSubsystem
{
	GENERATED_BODY()

	UDuoQDiscordSubsystem();

	bool UpdateActivity();

	bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void NativeOnDiscordCoreCreated() override;
	virtual void NativeOnDiscordCoreReset() override;

protected:
	UPROPERTY()
	FString LargeImageName = "baseicon";

	UPROPERTY()
	FString GameName = "DuoQ";

	UPROPERTY()
	FString LargeText = "";

	UPROPERTY()
	FString LobbyText = "";

	UPROPERTY()
	FString SmallImageName = "";

	UPROPERTY()
	FString SmallImageText = "DuoQ";

	int64 StartTime;

public:
	UFUNCTION(BlueprintCallable)
	bool SetLobbyText(FString Big, FString Small);
};
