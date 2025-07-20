// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Characters/ReviveManager.h"
#include "DuoQGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelsLoaded);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatManagerRegistered, class ACombatManager*, RegisteredManager);

USTRUCT(BlueprintType, Blueprintable)
struct FLevelBundle
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int StoryProgress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> Levels;
};

UCLASS(minimalapi)
class ADuoQGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADuoQGameMode();
	class AStarlightCharacter* Starlight;
	class ADuoQPlayerCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Revive")
	TSubclassOf<UReviveManager> ReviveManagerClass;

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	UReviveManager* ReviveManager;

	UFUNCTION(BlueprintCallable, Category = "Revive")
	UReviveManager* GetReviveManager() const;

	virtual AActor* ChoosePlayerStart_Implementation(AController* PlayerController) override;

	virtual AActor* FindPlayerStart_Implementation(AController* PlayerController, const FString& IncomingName) override;

	void AutoSelectScalability();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldState")
	TArray<FLevelBundle> Levels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldState")
	bool bIsLevelLoaded = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldState")
	int NumLevelsLoaded = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldState")
	int NumLevelsLoading = 0;
	UPROPERTY(EditAnywhere, BlueprintREadWRite, Category = "WorldState")
	FOnLevelsLoaded LevelLoaded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> FadeWidget;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* FadeWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "WorldState")
	TSubclassOf<class UMusicManager> Level0Music;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Manager")
	TArray<ACombatManager*> ActiveCombatManagers;

	UPROPERTY(BlueprintReadWrite, Category = "Combat Manager")
	bool bDiedViaCombat = false;

protected:
	virtual void BeginPlay() override; // Add BeginPlay override to initialize ReviveManager

public:
	UFUNCTION(BlueprintCallable)
	void OnLevelLoaded();

	UFUNCTION(BlueprintCallable)
	void RegisterCombatManager(ACombatManager* CombatManager);

	UFUNCTION(BlueprintCallable)
	void UnregisterCombatManager(ACombatManager* CombatManager);

	UFUNCTION(BlueprintCallable)
	const TArray<ACombatManager*>& GetActiveCombatManager() const;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnCombatManagerRegistered OnCombatManagerRegistered;
	

};



