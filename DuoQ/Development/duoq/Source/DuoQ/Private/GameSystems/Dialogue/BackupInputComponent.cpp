// Copyright 2024


#include "GameSystems/Dialogue/BackupInputComponent.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "MainMenuGameMode.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "UI/BackupCalloutWidget.h"
#include "UI/DialogueWidget.h"
#include "UI/GameplayHUD.h"
#include "UI/IntroSequenceHUD.h"

// Sets default values for this component's properties
UBackupInputComponent::UBackupInputComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UBackupInputComponent::BackupInputStart()
{
	BackupInputDirection = FVector2D::Zero();
	if(UDialogueSubsystem* DialogueSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>(); DialogueSubsystem && DialogueSubsystem->GetIsInConversation())
	{
		InputState = EBackupInputState::Dialogue;

		if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
		{
			DialogueWidget = DuoQPlayerCharacter->PlayerHUD->DialogueHUD;
		}
		else if (AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			DialogueWidget = MainMenuGameMode->MenuHUD->DialogueHUD;
		}
		
		if(DialogueWidget)
		{
			DialogueWidget->UpdateHighlight(FVector2D::Zero());
			DialogueWidget->SetHiddenOptionsVisibility(true);
		}
	}
	else
	{
		InputState = EBackupInputState::Callout;
		if (ADuoQPlayerCharacter* DuoQPlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
		{
			BackupCalloutWidget = DuoQPlayerCharacter->PlayerHUD->GameplayHUD->BackupCalloutWidget;
		}
		if(BackupCalloutWidget)
		{
			BackupCalloutWidget->DisplayOptions();
		}
	}
}

void UBackupInputComponent::BackupInputRelease()
{
	if(InputState == EBackupInputState::Dialogue)
	{
		if(DialogueWidget)
		{
			UDialogueOptionWidget* HoveredOption = DialogueWidget->CalculateSelectedOption(BackupInputDirection);
			if(BackupInputDirection.Length() >= BackupInputDeadzone && HoveredOption)
			{
				if(UDialogueSubsystem* DialogueSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
				{
					DialogueSubsystem->ActivateDollar(HoveredOption->GetOptionDollarName());
				}
			}
			else
			{
				DialogueWidget->SetHiddenOptionsVisibility(false);
			}
		}
		
	}
	else if(InputState == EBackupInputState::Callout)
	{
		if(BackupCalloutWidget)
		{
			if(BackupInputDirection.Length() >= BackupInputDeadzone * 2.0f)
			{
				BackupCalloutWidget->SelectHighlightedWidget(BackupInputDirection);
			}
			BackupCalloutWidget->HideOptions();
		}
	}

	BackupInputDirection = FVector2D::Zero();
}

void UBackupInputComponent::HandleBackupInput(FVector2D InputDirection)
{
	// crazy math or maybe just adding stuff
	BackupInputDirection += InputDirection;
	BackupInputDirection.X = FMath::Clamp(BackupInputDirection.X,-BackupInputBounds.X,BackupInputBounds.X);
	BackupInputDirection.Y = FMath::Clamp(BackupInputDirection.Y,-BackupInputBounds.Y,BackupInputBounds.Y);
	if(InputState == EBackupInputState::Dialogue)
	{
		HandleDialogueBackupInput();
	}
	else if(InputState == EBackupInputState::Callout)
	{
		HandleCalloutBackupInput();
	}
}

void UBackupInputComponent::HandleDialogueBackupInput()
{
	if(DialogueWidget)
	{
		DialogueWidget->UpdateCenterDot(BackupInputDirection);
		if(BackupInputDirection.Length() >= BackupInputDeadzone)
		{
			DialogueWidget->UpdateHighlight(BackupInputDirection);
		}
		else
		{
			DialogueWidget->HoverOption(nullptr);
		}
	}
}

void UBackupInputComponent::HandleCalloutBackupInput()
{
	if(BackupCalloutWidget)
	{
		BackupCalloutWidget->UpdateCenterDot(BackupInputDirection);
		if(BackupInputDirection.Length() >= BackupInputDeadzone * 2.0f)
		{
			BackupCalloutWidget->UpdateHighlight(BackupInputDirection);
		}
		else
		{
			BackupCalloutWidget->HighlightWidget(nullptr);
		}
	}
}
