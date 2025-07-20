// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API USaveSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
private:
	
	

public:
	bool bTalaHasGun = false;
	bool bPlayerHasGun = false;
	UFUNCTION(BlueprintCallable)
	void NewSave();
	UFUNCTION(BlueprintCallable)
	void Save(int PlayerProgress = 0, int StarlightAffinity = 0);
	UFUNCTION(BlueprintCallable)
	void SaveEnemy(FName enemy, bool status);
	UFUNCTION(BlueprintCallable)
	int DebugForward();
	UFUNCTION(BlueprintCallable)
	int DebugBackward();
	UFUNCTION(BlueprintCallable)
	int DebugToLevel(int TargetProgress);
	UFUNCTION(BlueprintCallable)
	int GetPlayerProgress();
	UFUNCTION(BlueprintCallable)
	int GetStarlightAffinity();
	UFUNCTION(BLueprintCallable)
	bool GetStarlightHasGun();
	UFUNCTION(BLueprintCallable)
	bool GetPlayerHasGun();
	UFUNCTION(BlueprintCallable)
	void SaveGun(bool PlayerGun, bool StarlightGun);
	UFUNCTION(BlueprintCallable)
	void SaveMenuSettings(float Master, float Music, float Chat, float SFX);
	
	UFUNCTION(BlueprintCallable)
	void SetGun(bool PlayerGun, bool StarlightGun);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> TriggeredEventToLoad;
};
