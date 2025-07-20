// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameplayHUD.h"

#include "EnhancedInputComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/OffscreenIndicatorWidget.h"

bool UGameplayHUD::Initialize()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Bind the input action
			EnhancedInputComponent->BindAction(InputAction_ToggleGameplayHUD, ETriggerEvent::Triggered, this, &UGameplayHUD::ToggleGameplayHUD);
			UE_LOG(LogDialogue, Log, TEXT("PlayerCharacter found, input bound for gameplay HUD."));
		}
		else
		{
			UE_LOG(LogDialogue, Warning, TEXT("Failed to cast PlayerController's InputComponent to EnhancedInputComponent."));
		}
	}
	
	return Super::Initialize();
}

void UGameplayHUD::ShowReviveText()
{
	ReviveText->SetVisibility(ESlateVisibility::Visible);
}

void UGameplayHUD::HideReviveText()
{
	ReviveText->SetVisibility(ESlateVisibility::Collapsed);
}

void UGameplayHUD::ShowOffscreenIndicator()
{
	if(OffscreenIndicatorWidget)
	{
		OffscreenIndicatorWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGameplayHUD::HideOffscreenIndicator()
{
	if(OffscreenIndicatorWidget)
	{
		OffscreenIndicatorWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGameplayHUD::ShowMissionComplete()
{
	if(MissionComplete)
	{
		MissionComplete->SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayMissionCompleteVideo();
	}
}

void UGameplayHUD::ToggleGameplayHUD()
{
	SetVisibility(GetVisibility() != ESlateVisibility::Visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
