// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DuoQHUD.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "UI/DQ_Pause_Menu.h"
#include "UI/GameplayHUD.h"
#include "UI/HealthBarPartner.h"
#include "GameSystems/Audio/AudioSubsystem.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"


bool UDuoQHUD::Initialize()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Bind the input action
			EnhancedInputComponent->BindAction(InputAction_ToggleHUD, ETriggerEvent::Triggered, this, &UDuoQHUD::ToggleHUD);
			UE_LOG(LogDialogue, Log, TEXT("PlayerCharacter found, input bound for gameplay HUD."));
		}
		else
		{
			UE_LOG(LogDialogue, Warning, TEXT("Failed to cast PlayerController's InputComponent to EnhancedInputComponent."));
		}
	}
	
	return Super::Initialize();
}

void UDuoQHUD::TogglePauseMenu()
{
	if (ADuoQPlayerController* Controller = UDuoQBlueprintFunctionLibrary::GetDuoQPlayerController(GetWorld(), 0))
	{
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			PauseMenu->SetVisibility(ESlateVisibility::Visible);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(Controller, PauseMenu, EMouseLockMode::DoNotLock, false);
			Controller->SetShowMouseCursor(true);
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			if (UAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
			{
				AudioSubsystem->ApplyPauseMix();
			}
			UGameplayStatics::PlaySound2D(GetWorld(), PauseSound);
		}
		else
		{
			PauseMenu->SetVisibility(ESlateVisibility::Collapsed);
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(Controller);
			Controller->SetShowMouseCursor(false);
			UGameplayStatics::SetGamePaused(GetWorld(), false);
			if (UAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
			{
				AudioSubsystem->RemovePauseMix();
			}
			UGameplayStatics::PlaySound2D(GetWorld(), UnpauseSound);
		}
	}
}

void UDuoQHUD::UpdateHealthBarPartner(float NewHealth, float MaxHealth)
{
	if (GameplayHUD->HealthBarPartner)
	{
		GameplayHUD->HealthBarPartner->UpdateProgressBar(NewHealth, MaxHealth);
	}
}

void UDuoQHUD::ToggleHUD()
{
	SetVisibility(GetVisibility() != ESlateVisibility::Visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}



