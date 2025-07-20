// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AchievementSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,Abstract)
class DUOQ_API UAchievementSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	UFUNCTION(BlueprintCallable)
	void UnlockAchievement(FString Achievement);

	UFUNCTION()
	void CheckAchievementUnlock(FString Event);

	UFUNCTION(BlueprintCallable)
	void SetUpAchievements();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void UnlockSteamAchievement(FName Achievement);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TMap<FString,FString> AchievementsList;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bExpoBuild = true;

	UFUNCTION(BlueprintCallable,BlueprintPure)
	APlayerController* GetPlayerController();
};
