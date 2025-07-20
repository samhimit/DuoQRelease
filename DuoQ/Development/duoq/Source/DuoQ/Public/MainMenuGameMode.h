// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"


/**
 * 
 */
UCLASS()
class DUOQ_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UIntroSequenceHUD> MenuHUD;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> MonitorWidget;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UUserWidget> MainMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class AMainMenuPawn> CameraActor;
};
