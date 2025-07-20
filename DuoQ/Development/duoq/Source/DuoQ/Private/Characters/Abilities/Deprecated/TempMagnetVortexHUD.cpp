// Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/Abilities/Deprecated/TempMagnetVortexHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Engine/Engine.h"

void ATempMagnetVortexHUD::DrawHUD()
{
	Super::DrawHUD();


    if (!Canvas)
	{
		return;
	}

	// Text to display
	FString ChargeText = FString::Printf(TEXT("Magnet Vortex Charge: %.1f"), CurrentCharge);

	// Set the scale for larger text
	float TextScale = 3.0f; // Increase this value to make the text larger

	// Adjust the position: Bottom right corner, a little more to the left
	float ScreenX = Canvas->ClipX - 750.0f; // Moved more to the left
	float ScreenY = Canvas->ClipY - 70.0f;	// Adjusted Y position slightly

	// Create a text item with the desired properties
	FCanvasTextItem TextItem(FVector2D(ScreenX, ScreenY), FText::FromString(ChargeText), GEngine->GetLargeFont(), FLinearColor::Green);

	// Apply the scale for larger text
	TextItem.Scale = FVector2D(TextScale, TextScale);

	// Optional: Set text shadow for better visibility
	TextItem.bOutlined = true;
	TextItem.OutlineColor = FLinearColor::Black;

	// Draw the text item on the screen
	Canvas->DrawItem(TextItem);
}