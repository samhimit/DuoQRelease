#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TempMagnetVortexHUD.generated.h"

/**
 * A custom HUD class to display the current charge on the screen.
 */
UCLASS()
class DUOQ_API ATempMagnetVortexHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Override the DrawHUD function to render custom HUD elements
	virtual void DrawHUD() override;

	// Variable to hold the current charge value that will be displayed on the screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagnetVortex")
	float CurrentCharge;
};
