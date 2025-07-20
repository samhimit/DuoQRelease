// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/DuoQPlayerController.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "UI/HealthBarPartner.h"
#include "UI/OffscreenIndicatorWidget.h"

void ADuoQPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (OffscreenIndicatorClass)
	{
		OffscreenIndicatorWidget = CreateWidget<UOffscreenIndicatorWidget>(this, OffscreenIndicatorClass);
		if (OffscreenIndicatorWidget)
		{
			OffscreenIndicatorWidget->AddToViewport();
		}
	}

	Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld());
}

void ADuoQPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Starlight)
	{
		UpdateOffscreenIndicator(Starlight);
	}
	else
	{
		Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld());
	}
}

void ADuoQPlayerController::UpdateOffscreenIndicator(AStarlightCharacter* InStarlight)
{
	if (InStarlight && InStarlight->IsValidLowLevel() && OffscreenIndicatorWidget)
	{
		FVector StarlightLocation = InStarlight->GetActorLocation();
		FVector CameraLocation = PlayerCameraManager->GetCameraLocation();

		FVector Direction = (StarlightLocation - CameraLocation).GetSafeNormal();

		FVector2D ScreenPos;

		UGameplayStatics::ProjectWorldToScreen(this, StarlightLocation, ScreenPos);

		float ScreenWidth = GEngine->GameViewport->Viewport->GetSizeXY().X;
		float ScreenHeight = GEngine->GameViewport->Viewport->GetSizeXY().Y;

		//Starlight is on screen based on if her projected screen loc is out of bounds
		bool bIsOnScreen = (ScreenPos.X <= ScreenWidth && ScreenPos.X >= 0.0f) && (ScreenPos.Y <= ScreenHeight && ScreenPos.Y >= 0.0f) && (!ScreenPos.IsNearlyZero());

		if (bIsOnScreen)
		{
			OffscreenIndicatorWidget->SetIndicatorPosition(ScreenPos, false);
			if(UDuoQHUD* HUD = UDuoQBlueprintFunctionLibrary::GetHUDInstance(this))
			{
				HUD->GamerTag->SetVisibility(ESlateVisibility::Visible);
			}
		}

		else if (ADuoQGameMode* GM = UDuoQBlueprintFunctionLibrary::GetDuoQGameMode(GetWorld()))
		{
			if (UReviveManager* RM = GM->GetReviveManager())
			{
				if (RM->StarlightState != EState::Dead)
				{
					if(UDuoQHUD* HUD = UDuoQBlueprintFunctionLibrary::GetHUDInstance(this))
					{
						HUD->GamerTag->SetVisibility(ESlateVisibility::Collapsed);
					}
			
					// get object camera space
					FTransform CameraTransform = PlayerCameraManager->GetTransform();
					FTransform InverseCameraTransform = CameraTransform.Inverse();
					FVector    StarlightInCameraSpace = InverseCameraTransform.TransformPosition(StarlightLocation);

					float Angle = FMath::Atan2(StarlightInCameraSpace.Z, StarlightInCameraSpace.Y);

					FVector2D EdgePos;

					//x pos based on angle
					EdgePos.X = (FMath::Cos(Angle) * (ScreenWidth / 2.0f)) + (ScreenWidth / 2.0f);

					//EdgePos.Y = (FMath::Sin(Angle) * (ScreenHeight / 2.0f)) + (ScreenHeight / 2.0f);
					//EdgePos.Y = ScreenHeight - EdgePos.Y;

					//adding extra camera space check overwrites any bIsOnScreen errors, for some reason

					//starlight above/below
					if (StarlightInCameraSpace.Z > 0.0f)
					{
						EdgePos.Y = (FMath::Sin(Angle) * (ScreenHeight / 2.0f)) + (ScreenHeight / 2.0f);
						EdgePos.Y = ScreenHeight - EdgePos.Y;
					}
					else
					{
						EdgePos.Y = (FMath::Sin(Angle) * (ScreenHeight / 2.0f)) + (ScreenHeight / 2.0f);
						EdgePos.Y = ScreenHeight - EdgePos.Y;
					}

					//invert again if pitch is up to keep hud at bottom, if Starlight is not above
					if (CameraTransform.Rotator().Pitch > 0.0f)
					{
						if (GetOwner())
						{
							if (StarlightInCameraSpace.Z <= 0.0f)
							{
								EdgePos.Y = ScreenHeight - EdgePos.Y;
							}
						}
					}

					//offset to keep on screen
					if (EdgePos.X > (ScreenWidth - EdgeOffset))
					{
						EdgePos.X = ScreenWidth - EdgeOffset;// * 5.0f;
					}

					if (EdgePos.Y > (ScreenHeight - EdgeOffset))
					{
						EdgePos.Y = ScreenHeight - EdgeOffset;
					}
					const float ClampedX = EdgePos.X;
					int32 ScreenX, ScreenY;
					GetViewportSize(ScreenX, ScreenY);
					const float WidgetSize = OffscreenIndicatorWidget->GetCachedGeometry().GetLocalSize().X;
					EdgePos.X = FMath::Clamp(ClampedX, 0.0f, static_cast<float>(ScreenX) - WidgetSize * 2.0);
					OffscreenIndicatorWidget->SetIndicatorPosition(EdgePos, true);
				}
			}

		}
	}

}


