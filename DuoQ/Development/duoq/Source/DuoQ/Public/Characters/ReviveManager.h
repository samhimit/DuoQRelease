// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/DuoQCharacter.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Misc/OutputDeviceNull.h"
#include "TimerManager.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ReviveManager.generated.h"

/**
 * Enum to represent Players or Starlight's state (Alive or Dead).
 */
UENUM(BlueprintType)
enum class EState : uint8
{
	Alive UMETA(DisplayName = "Alive"),
	Dead  UMETA(DisplayName = "Dead")
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverDelegate);

UCLASS(Blueprintable, BlueprintType)
class DUOQ_API UReviveManager : public UObject
{
	GENERATED_BODY()

public:
	UReviveManager();
	UFUNCTION()
	void ClearTimer(UWorld* world, bool tick, bool deltaTime);

	// Variables
	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	EState StarlightState; // Tracks Starlight's current state (Alive or Dead)

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	float SoulPickupTime; // Amount of time needed to stand near Starlight�s soul to pick it up

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	float ReviveTime; // Amount of time needed to revive the player at the revive location

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	float AutoReviveTime;

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	bool bPlayerHasSoul; // Tracks if the player has Starlight�s soul

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	bool bStarlightHasSoul; // Tracks if the player has Starlight�s soul

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	EState PlayerState; // Tracks the player's state (Alive or Dead)

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	FRotator PlayerCameraRotation; // Camera rotation when spectating

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	FVector PlayerCameraLocation; // Camera location when spectating

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	float ReviveHealth; // The proportion of health a player revives with (0-1 scale)

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	bool bBothPlayersDead; // Tracks if both players are dead to initiate death screen


	UPROPERTY(BlueprintAssignable, Category = "Revive")
	FOnGameOverDelegate OnGameOver;

	// Widget reference for the fade effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> FadeWidget; // Class for the fade-to-black widget

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ReviveWidget; // Class for the fade-to-black widget

	// Widget reference for the fade effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidget; // Class for the fade-to-black widget with "Game Over" on it.

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* FadeWidgetInstance; // Instance of the fade widget
	
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* GameOverInstance; // Instance of the game over widget

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* ReviveInstance; // Instance of the revive widget

	UFUNCTION(BlueprintCallable, Category = "Revive")
	void HandleDeath(ADuoQCharacter* Character);

	UPROPERTY(EditAnywhere, Category = "Revive")
	USoundBase* ReviveSound;

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	float FadeTimeCameraSwitch;

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	float InteractTime;

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	AActor* PlayerSoul = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Revive")
	AActor* StarlightSoul = nullptr;

	UFUNCTION()
	void ReviveStarlight(bool timer = false);

	UFUNCTION()
	void RevivePlayer(bool timer = false);

protected:
	
	UFUNCTION()
	void GameOver();

private:
	UPROPERTY()
	ADuoQPlayerCharacter* Starlight;

	UPROPERTY()
	FTimerHandle ReviveTimerHandle;

	UPROPERTY()
	FTimerHandle AutoPlayerReviveTimerHandle;

	UPROPERTY()
	FTimerHandle AutoStarlightReviveTimerHandle;

	UPROPERTY()
	FTimerHandle SwitchCameraHandle;
	UPROPERTY()
	FTimerHandle GameOverTimerHandle;
	UPROPERTY()
	float ReviveTimer;
	UPROPERTY()
	float ReviveTimerMax;

	UFUNCTION()
	void FadeToBlack(APlayerController* PlayerController);

	UFUNCTION()
	void RemoveFadeToBlack();

	UFUNCTION()
	void TeleportStarlightToPlayer();

	UFUNCTION()
	void TeleportPlayerToStarlight();

	UFUNCTION()
	void SwitchCamera(APlayerController* PlayerController);

	UFUNCTION()
	void SwitchCameraBack(APlayerController* PlayerController);

	UFUNCTION()
	void AutoStarlightReviveWrapper();

	UFUNCTION()
	void AutoPlayerReviveWrapper();
	UFUNCTION()
	void ReviveUIUpdate(UUserWidget* src);
	UFUNCTION()
	void ReviveSingleUIUpdate();
	UFUNCTION()
	void ReviveBothUIUpdate();

	UFUNCTION(BlueprintCallable)
	AActor* FindPlayerStartForController(AController* Controller);
};
