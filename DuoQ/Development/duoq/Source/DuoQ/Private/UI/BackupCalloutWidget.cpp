// Copyright 2024


#include "UI/BackupCalloutWidget.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameSystems/Communication/CommunicationSubsystem.h"
#include "UI/BackupCalloutOptionWidget.h"
#include "UI/CalloutHeartArrowWidget.h"

void UBackupCalloutWidget::DisplayOptions()
{
	if(UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		if(AStarlightGOAPController* StarlightGoapController = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(this))
		{
			TArray<FAtom> Goals;
			if(CommunicationSubsystem->bUseHumanReadableGoals)
			{
				FScopeLock Lock(&CommunicationSubsystem->BackupCommandsGuard);
				for(auto Command : CommunicationSubsystem->BackupCommands)
				{
					Goals.Emplace(Command,true);
				}
			}
			else
			{
				StarlightGoapController->GetGoals().GetKeys(Goals);
			}
		
			int NumberOfGoals = Goals.Num();

			int RightSideGoalsCount = Goals.Num() / 2;
			int LeftSideGoalsCount = RightSideGoalsCount + (NumberOfGoals % 2);

			float RightSideAngle = (PI / (RightSideGoalsCount + 1));
			float LeftSideAngle = (PI / (LeftSideGoalsCount + 1));
		
			for(int i = 0; i < NumberOfGoals; i++)
			{
				if(RightSideOptionWidgetClass)
				{
					if(UBackupCalloutOptionWidget* Widget = CreateWidget<UBackupCalloutOptionWidget>(GetWorld(),  i % 2 == 0 ? LeftSideOptionWidgetClass : RightSideOptionWidgetClass))
					{
						Widget->OptionText->SetText(FText::FromString(Goals[i].name));
						Widget->Goal = Goals[i];
						Widget->CalloutText = Goals[i].name;
					
						UCanvasPanelSlot* WidgetSlot = CanvasPanel->AddChildToCanvas(Widget);

						WidgetSlot->SetAnchors(FAnchors(0.5f,0.5f,0.5f,0.5f));
						WidgetSlot->SetAlignment(FVector2D(0.5f, 0.5f));
						WidgetSlot->SetPosition(FVector2D(0.0f,0.0f));

						float AddedAngle = ((i/2)+1) * ((i % 2 == 0) ? LeftSideAngle : -RightSideAngle);
						float BaseAngle = PI / 2.0f;

						float XLocation = FMath::Cos(BaseAngle + AddedAngle)* CircleRadius;
						float YLocation = FMath::Sin(BaseAngle + AddedAngle) * CircleRadius;
						Widget->SetRenderTranslation(FVector2D(XLocation, YLocation));
					
						OptionWidgets.Add(Widget);

						if(UCalloutHeartArrowWidget* HeartWidget = CreateWidget<UCalloutHeartArrowWidget>(GetWorld(), HeartWidgetClass))
						{
							UCanvasPanelSlot* HeartWidgetSlot = CanvasPanel->AddChildToCanvas(HeartWidget);
							HeartWidgetSlot->SetAnchors(FAnchors(0.5f,0.5f,0.5f,0.5f));
							HeartWidgetSlot->SetAlignment(FVector2D(0.5f, 0.5f));
							HeartWidgetSlot->SetPosition(FVector2D(0.0f,0.0f));
							
							float HeartLocationX = FMath::Cos(BaseAngle + AddedAngle)* HeartRadius;
							float HeartLocationY = FMath::Sin(BaseAngle + AddedAngle) * HeartRadius;
							HeartWidget->SetRenderTranslation(FVector2D(HeartLocationX,HeartLocationY));
							HeartWidget->SetRenderTransformAngle(FMath::RadiansToDegrees(BaseAngle+AddedAngle));

							Widget->OptionArrow = HeartWidget;
							HeartImages.Add(HeartWidget);
						}
						
					}
				}
			}
		}
		FWidgetTransform Transform = CenterDot->GetRenderTransform();
		Transform.Translation = FVector2D::Zero();
		CenterDot->SetRenderTransform(Transform);
		
		SetVisibility(ESlateVisibility::Visible);
	}
}

void UBackupCalloutWidget::HideOptions()
{
	for(auto Widget : OptionWidgets)
	{
		if(Widget)
		{
			Widget->RemoveFromParent();
		}
	}
	for(auto Widget : HeartImages)
	{
		if(Widget)
		{
			Widget->RemoveFromParent();
		}
	}
	HighlightedWidget = nullptr;
	HeartImages.Empty();
	OptionWidgets.Empty();
	SetVisibility(ESlateVisibility::Collapsed);
}

UBackupCalloutOptionWidget* UBackupCalloutWidget::CalculateSelectedOption(FVector2D Direction)
{
	Direction.Normalize();
	
	int IsOnTheRight = Direction.X >= 0.0f;

	int NumberOfOptionsPerCol = OptionWidgets.Num() / 2.0f + (OptionWidgets.Num() % 2 && !IsOnTheRight);

	float AngleAreaPerOption = PI / (NumberOfOptionsPerCol);
	
	float Angle = FMath::Atan2(Direction.Y,Direction.X);

	float BaseAngle = 3.0f * PI / 2.0f;
	
	// get into our "coordinate space"
	Angle += BaseAngle;
	
	// Normalize
	if(Angle < -PI)
	{
		Angle += PI * 2;
	}
	else if(Angle > PI)
	{
		Angle -= PI * 2;
	}

	UE_LOG(LogTemp,Display,TEXT("%f"),Angle);

	
	
	float UntruncatedOption = abs(Angle) / AngleAreaPerOption;

	int Option = UntruncatedOption;

	Option = (Option * 2) + IsOnTheRight;

	if(OptionWidgets.IsValidIndex(Option))
	{
		return OptionWidgets[Option];
	}
	return nullptr;
}

void UBackupCalloutWidget::HighlightWidget(UBackupCalloutOptionWidget* Widget)
{
	if(Widget != HighlightedWidget)
	{
		if(HighlightedWidget)
		{
			HighlightedWidget->SetHighlight(FColor::White,FColor::Black);
		}
		if(Widget)
		{
			Widget->SetHighlight(FColor::Purple,FColor::White);
		}
		HighlightedWidget = Widget;
	}
}

void UBackupCalloutWidget::UpdateHighlight(FVector2D Direction)
{
	UBackupCalloutOptionWidget* NewHighlight = CalculateSelectedOption(Direction);
	HighlightWidget(NewHighlight);
}

void UBackupCalloutWidget::UpdateCenterDot(FVector2D Direction)
{
	if(CenterDot)
	{
		FWidgetTransform Transform = CenterDot->GetRenderTransform();
		Transform.Translation = Direction;
		CenterDot->SetRenderTransform(Transform);
	}
}

void UBackupCalloutWidget::SelectHighlightedWidget(FVector2D Direction)
{
	if(UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		if(UBackupCalloutOptionWidget* BackupCalloutOptionWidget = CalculateSelectedOption(Direction))
		{
			if(CommunicationSubsystem->bUseHumanReadableGoals)
			{
			
				CommunicationSubsystem->DebugStringInsert(BackupCalloutOptionWidget->CalloutText);
			}
			else
			{
				if(AStarlightGOAPController* StarlightGoapController = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(this))
				{
					for(auto& Keypair : StarlightGoapController->GetGoals())
					{
						if(Keypair.Value <= 1)
						{
							Keypair.Value = 2;
						}
					}
					StarlightGoapController->SetDesiredWorldAtomPriority(BackupCalloutOptionWidget->Goal,1);
				}
			}
		}
	}
}