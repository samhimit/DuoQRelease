// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "MusicManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class DUOQ_API UMusicManager : public UAudioComponent
{
	GENERATED_BODY()

	UMusicManager();
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnCreate(float FadeInTime);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStop(float FadeTime);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AddLayer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RemoveLayer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Reset();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEnemySeen(int EnemyCount);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEnemyKilled(int EnemyCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAmbient;

	UPROPERTY(EditDefaultsOnly)
	bool bUsesTransition = false;
};
