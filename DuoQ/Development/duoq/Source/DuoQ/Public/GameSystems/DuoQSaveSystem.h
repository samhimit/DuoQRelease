// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "DuoQSaveSystem.generated.h"

/**
 *
 */
UCLASS()
class DUOQ_API UDuoQSaveSystem : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	FString SaveSlotName;
	// How far along is the player (Which room have they progressed to)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	int PlayerProgress = -1;
	// The affinity level of starlight.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	int StarlightAffinity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	bool PlayerHasGun = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	bool StarlightHasGun = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	TMap<FName, bool> EnemyStatus;

	/** only keeps track of which collectable is collected*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	TMap<FName, bool> SavedCollectables;

	/** keeps track of all the saved events upon exit*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	TArray<FString> SavedTriggeredDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	EPlayerEncounter SavedEncounter = EPlayerEncounter::FirstShielded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	float MasterVolume = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	float ChatVolume = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	float MusicVolume = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	float SFXVolume = 0.5f;

	UDuoQSaveSystem();
};
