// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayHUD.generated.h"

class UBackupCalloutWidget;
class UOffscreenIndicatorWidget;
class UGunChargeWidget;
class UAbilityWidget;
class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class DUOQ_API UGameplayHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UAbilityWidget> ShieldWidget;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UGunChargeWidget> GunChargeWidget;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ReviveText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UOffscreenIndicatorWidget> OffscreenIndicatorWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UBackupCalloutWidget> BackupCalloutWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UHealthBarPartner> HealthBarPartner;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> Crosshair;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCollectableWidget> CollectableWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UWidget> LeaveNotification;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UWidget> MissionComplete;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UWidget> ChestItemWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim),Transient)
	TObjectPtr<class UWidgetAnimation> MissionCompleteAnimation;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayMissionCompleteVideo();
	
	UFUNCTION(BlueprintCallable)
	void ShowReviveText();

	UFUNCTION(BlueprintCallable)
	void HideReviveText();
	
	UFUNCTION(BlueprintCallable)
	void ShowOffscreenIndicator();

	UFUNCTION(BlueprintCallable)
	void HideOffscreenIndicator();

	UFUNCTION(BlueprintImplementableEvent)
	void TalaLeaveGame();

	UFUNCTION(BlueprintCallable)
	void ShowMissionComplete();
	
	// Input action for hiding gameplay HUD
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	UInputAction* InputAction_ToggleGameplayHUD;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	
	// Function to toggle the gameplay HUD
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ToggleGameplayHUD();
};
