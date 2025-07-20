// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DialogueWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "UI/DialogueOptionWidgetFlipped.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDialogueWidget::ShowOptions(const TArray<FDollar>& Dollars, bool bOpenEnded)
{
	bIsInOpenEnded = bOpenEnded;
	// If there is no options just do nothing and return
	if(Dollars.Num() == 0)
	{
		return;
	}
	const int TotalOptions = Dollars.Num();
	if (TotalOptions > 1)
	{
		CenterDot->SetVisibility(ESlateVisibility::Visible);
		PlayFadeCenterDot(false);
		FWidgetTransform Transform = CenterDot->GetRenderTransform();
		Transform.Translation = FVector2D::ZeroVector;
		CenterDot->SetRenderTransform(Transform);
	}


	Algo::Sort(Dollars,[](const FDollar& Dollar1, const FDollar& Dollar2) {
		return Dollar1.Cooldown < Dollar2.Cooldown;
	});
	
	if (false)
	{
		for(int32 i = 0; i < TotalOptions;++i)
		{
			if (i == 0)
			{
				UDialogueOptionWidget* OptionWidget = CreateWidget<UDialogueOptionWidget>(GetWorld(), OptionWidgetClass);
				if(OptionWidget)
				{
					OptionWidget->SetOptionText(Dollars[i].OptionText);
					OptionWidget->SetOptionDollarName(Dollars[i].Name);
					OptionWidgets.Add(OptionWidget);
			
					// Set padding for the slot
					if (UHorizontalBoxSlot* OptionSlot = OptionsHorizontalBox->AddChildToHorizontalBox(OptionWidget))
					{
						OptionSlot->SetPadding(FMargin(OptionsHorizontalMargin, 0.0f));
					}
				}
			}
			else if (i == 1)
			{
				UDialogueOptionWidgetFlipped* OptionWidget = CreateWidget<UDialogueOptionWidgetFlipped>(GetWorld(), OptionWidgetFlippedClass);
				if(OptionWidget)
				{
					OptionWidget->SetOptionText(Dollars[i].OptionText);
					OptionWidget->SetOptionDollarName(Dollars[i].Name);
					OptionWidgets.Add(OptionWidget);
					
			
					// Set padding for the slot
					if (UHorizontalBoxSlot* OptionSlot = OptionsHorizontalBox->AddChildToHorizontalBox(OptionWidget))
					{
						OptionSlot->SetPadding(FMargin(OptionsHorizontalMargin, 0.0f));
					}
				}
			}
		}
	}
	else
	{
		for(int32 i = 0; i < TotalOptions;++i)
		{
			if((FMath::IsNearlyZero(Dollars[i].Cooldown) && !bIsInOpenEnded && OptionWidgets.Num() < 4) || (!FMath::IsNearlyZero(Dollars[i].Cooldown) && bIsInOpenEnded))
			{
				UDialogueOptionWidget* OptionWidget;
				// we can't use i because we're not inserting every widget into it
				if (OptionWidgets.Num() % 2 == 0)
				{
					OptionWidget = CreateWidget<UDialogueOptionWidget>(GetWorld(), OptionWidgetClass);
					if(OptionWidget)
					{
						// Set padding for the slot
						if (UVerticalBoxSlot* OptionSlot = OptionsLeftVerticalBox->AddChildToVerticalBox(OptionWidget))
						{
							OptionSlot->SetPadding(FMargin(0.0f, OptionsVerticalMargin));
						}
					}
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("OptionWidgetFlippedClass: %s"), *OptionWidgetFlippedClass->GetName());
					OptionWidget = CreateWidget<UDialogueOptionWidgetFlipped>(GetWorld(), OptionWidgetFlippedClass);
					if(OptionWidget)
					{
						// Set padding for the slot
						if (UVerticalBoxSlot* OptionSlot = OptionsRightVerticalBox->AddChildToVerticalBox(OptionWidget))
						{
							OptionSlot->SetPadding(FMargin(0.0f, OptionsVerticalMargin));
						}
					}
				}
				
				if(OptionWidget)
				{
					OptionWidget->SetOptionText(Dollars[i].OptionText);
					OptionWidget->SetOptionDollarName(Dollars[i].Name);
					OptionWidgets.Add(OptionWidget);
					if(bIsInOpenEnded)
					{
						//PlayFadeOptions1(true);
						OptionWidget->SetVisibility(ESlateVisibility::Collapsed);
					}
				}
			}
			// open-ended one with the actual prompt
			else if(bIsInOpenEnded)
			{
				// display it as an option
				OpenEndedPrompt->SetOptionText(Dollars[i].OptionText);
				OpenEndedPrompt->SetVisibility(ESlateVisibility::HitTestInvisible);
				PlayFadeOptions1(false);
			}
			// normal hidden options
			else if(!bIsInOpenEnded)
			{
				HiddenOptionWidgets.Add(Dollars[i]);
			}
		}
	}
	//SaySomethingText->SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayFadeOptions3(false);
}

void UDialogueWidget::DelayShowOptions(const TArray<FDollar>& Dollars, bool bOpenEnded)
{
	FTimerHandle Handle;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, FName("ShowOptions"), Dollars, bOpenEnded);
	GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, 0.25f, false);
}

void UDialogueWidget::SetHiddenOptionsVisibility(bool bIsVisible)
{
	if(bIsInOpenEnded)
	{
		int i = 0;
		for(auto Widget : OptionWidgets)
		{
			i++;
			if(i > MaxDialogueOptions)
			{
				break;
			}
			Widget->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
		//PlayFadeCenterDot(bIsVisible ? false : true);
		CenterDot->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		//PlayFadeOptions1(bIsVisible ? true : false);
		OpenEndedPrompt->SetVisibility(bIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
		
	}
}

void UDialogueWidget::HideOptions()
{
	
	for(UDialogueOptionWidget* OptionWidget: OptionWidgets)
	{
		if(OptionWidget)
		{
			//problem with fading if dialogue occurs back to back
			PlayFadeOptions3(true);
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, OptionWidget, &UDialogueWidget::RemoveFromParent, 0.25f, false);
			//OptionWidget->RemoveFromParent();
		}
	}
	OptionWidgets.Empty();
	HighlightedWidget = nullptr;
	//SaySomethingText->SetVisibility(ESlateVisibility::Collapsed);
	//PlayFadeOptions1(true);
	PlayFadeCenterDot(true);
	OpenEndedPrompt->SetVisibility(ESlateVisibility::Collapsed);
	HiddenOptionReveal->SetVisibility(ESlateVisibility::Collapsed);
	CenterDot->SetVisibility(ESlateVisibility::Collapsed);
}

void UDialogueWidget::UpdateHighlight(FVector2D Direction)
{
	if(Direction == FVector2D::Zero())
	{
		HoverOption(nullptr);
	}
	else
	{
		HoverOption(CalculateSelectedOption(Direction));
	}
	
}

void UDialogueWidget::UpdateCenterDot(FVector2D Direction)
{
	if(CenterDot)
	{
		FWidgetTransform Transform = CenterDot->GetRenderTransform();
		Transform.Translation = Direction;
		CenterDot->SetRenderTransform(Transform);
	}
}

void UDialogueWidget::HoverOption(UDialogueOptionWidget* NewHoveredWidget)
{
	if(NewHoveredWidget != HighlightedWidget)
	{
		if(HighlightedWidget)
		{
			HighlightedWidget->HighlightOption(FColor::White);
		}
		if(NewHoveredWidget)
		{
			NewHoveredWidget->HighlightOption(FColor::Purple);
		}
		HighlightedWidget = NewHoveredWidget;
	}
}

UDialogueOptionWidget* UDialogueWidget::CalculateSelectedOption(FVector2D Direction)
{
	Direction.Normalize();
	
	int IsOnTheRight = Direction.X >= 0.0f;

	float NormalizedY = (Direction.Y + 1) / 2.0f;
	
	int NumberOfOptionsPerCol = OptionWidgets.Num() / 2.0f + (OptionWidgets.Num() % 2 && !IsOnTheRight);

	NumberOfOptionsPerCol = FMath::Clamp(NumberOfOptionsPerCol,0,MaxDialogueOptions/2);

	float AreaPerOption = 1.0f / NumberOfOptionsPerCol;

	float UntruncatedOption = NormalizedY / AreaPerOption;

	int Option = UntruncatedOption;

	Option = (Option * 2) + IsOnTheRight;

	if(OptionWidgets.IsValidIndex(Option))
	{
		return OptionWidgets[Option];
	}
	return nullptr;
}


void UDialogueWidget::HighlightOption(const FString& DollarName)
{
	bool bFoundOption = false;
	for(UDialogueOptionWidget* OptionWidget: OptionWidgets)
	{
		if(OptionWidget)
		{
			if (DollarName == OptionWidget->GetOptionDollarName())
			{
				OptionWidget->HighlightOption(FColor::Green);
				bFoundOption = true;
			}
		}
	}
	if(!bFoundOption)
	{
		for(FDollar Dollar : HiddenOptionWidgets)
		{
			if(Dollar.Name == DollarName)
			{
				HiddenOptionReveal->SetVisibility(ESlateVisibility::HitTestInvisible);
				HiddenOptionReveal->SetOptionText(Dollar.OptionText);
				HiddenOptionReveal->HighlightOption(FColor::Green);
			}
		}
	}
}
