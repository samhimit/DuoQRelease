// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DuoQHUD.generated.h"

class UInputAction;
/**
 *
 */
UCLASS()
class DUOQ_API UDuoQHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UGameplayHUD> GameplayHUD;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UDialogueWidget> DialogueHUD;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class USAMWidget> SAMHUD;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UControlsWidget> ControlsWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UDQ_Pause_Menu> PauseMenu;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UUserWidget> FadeToBlack;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UUserWidget> FadeScreen;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UUserWidget> GameOverScreen;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UUserWidget> GamerTag;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundBase> PauseSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundBase> UnpauseSound;

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable)
	void UpdateHealthBarPartner(float NewHealth, float MaxHealth);
	
	// Input action for dialogue progression
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	UInputAction* InputAction_ToggleHUD;
	
	UFUNCTION(BlueprintCallable)
	void ToggleHUD();

	UFUNCTION(BlueprintImplementableEvent)
	void RevertStarlight();
};
