// Copyright 2024


#include "UI/OffscreenIndicatorWidget.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Player/DuoQPlayerController.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameSystems/Dialogue/StarlightResponseSubsystem.h"

void UOffscreenIndicatorWidget::SetIndicatorPosition(FVector2D Position, bool bShouldBeVisible)
{
	
	if (UStarlightResponseSubsystem* SRS = UDuoQBlueprintFunctionLibrary::GetDuoQGameInstance(GetWorld())->GetSubsystem<UStarlightResponseSubsystem>())
	{
		
		if (bShouldBeVisible && !bShouldBeHiddenPermanently)
		{
			if (SRS->GetStarlightTalking() && GetViewportX() != -1)
			{
				SpeakerRight->SetVisibility(ESlateVisibility::Visible);
			}
			else if (!SRS->GetStarlightTalking())
			{
				SpeakerRight->SetVisibility(ESlateVisibility::Collapsed);
			}
			IndicatorText->SetVisibility(ESlateVisibility::Visible);
		
			SetPositionInViewport(Position);
		}
		else
		{
			IndicatorText->SetVisibility(ESlateVisibility::Collapsed);
			SpeakerRight->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UOffscreenIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IndicatorText)
	{
		IndicatorText->SetVisibility(ESlateVisibility::Visible);
	}
}

int32 UOffscreenIndicatorWidget::GetViewportX()
{
	if (ADuoQPlayerController* Controller = UDuoQBlueprintFunctionLibrary::GetDuoQPlayerController(GetWorld(), 0))
	{
		int32 ScreenX, ScreenY;
		Controller->GetViewportSize(ScreenX, ScreenY);
		return ScreenX;
	}
	return -1;
}

