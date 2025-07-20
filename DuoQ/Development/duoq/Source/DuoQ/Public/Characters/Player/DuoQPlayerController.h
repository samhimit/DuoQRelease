// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DuoQPlayerController.generated.h"

class UHealthBarPartner;
class UOffscreenIndicatorWidget;
class AStarlightCharacter;
/**
 * 
 */
UCLASS()
class DUOQ_API ADuoQPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	//could change to being any actor in the future, but for now just tracks Starlight
	void UpdateOffscreenIndicator(AStarlightCharacter* InStarlight);
	
	UOffscreenIndicatorWidget* GetOffscreenIndicatorWidget() { return OffscreenIndicatorWidget; }


protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UOffscreenIndicatorWidget> OffscreenIndicatorClass;

	UPROPERTY()
	UOffscreenIndicatorWidget* OffscreenIndicatorWidget;

	// how far the indicator will be from the edge of the screen
	UPROPERTY(EditAnywhere, Category = "UI")
	float EdgeOffset = 50.0f;

	UPROPERTY(EditAnywhere, Category = "UI")
	float MaxEdgeX = 2250.0f;

	UPROPERTY()
	AStarlightCharacter* Starlight;

	
	
	
	
};
