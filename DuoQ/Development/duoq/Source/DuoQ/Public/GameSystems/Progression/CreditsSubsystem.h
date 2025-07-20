// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CreditsSubsystem.generated.h"

class UCreditsWidget;

USTRUCT(Blueprintable, BlueprintType)
struct FCredit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Names;
};

USTRUCT(Blueprintable,BlueprintType)
struct FCreditPage : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCredit> Credits;
};

/**
 * 
 */
UCLASS(Blueprintable,Abstract)
class DUOQ_API UCreditsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	// For the fade out before credits screen
	FTimerHandle FadeOutTimerHandle;

	UFUNCTION()
	void FadeOutFromGameplay();

	UFUNCTION()
	void TransitionToMenu();

public:
	UFUNCTION(BlueprintCallable)
	void PlayEndGameCameraSequence();

	UFUNCTION(BlueprintCallable)
	void StartCreditsSequence(FString EventName);

	UFUNCTION(BlueprintCallable)
	void StartCreditsFromMenu(FString EventName);

	UFUNCTION(BlueprintCallable)
	void DisplayCreditsOnMonitor();

	UFUNCTION(BlueprintCallable)
	void PlayCredits();

	UPROPERTY(BlueprintReadOnly)
	bool bIsPlayingCredits = false;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* Texture;
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "UI")
	UUserWidget* FadingScreenInstance;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> FadingScreenClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "UI")
	UCreditsWidget* CreditsScreenInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "UI")
	TSubclassOf<UCreditsWidget> CreditsScreenClass;
};
