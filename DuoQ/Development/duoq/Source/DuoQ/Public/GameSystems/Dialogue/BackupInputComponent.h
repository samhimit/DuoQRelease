// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BackupInputComponent.generated.h"
struct FInputActionValue;
class UDialogueWidget;
class UBackupCalloutWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackupInputPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackupInputReleased);

UENUM()
enum EBackupInputState : uint8
{
	Dialogue,
	Callout
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UBackupInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBackupInputComponent();

	void BackupInputStart();

	void BackupInputRelease();

	void HandleBackupInput(FVector2D InputDirection);

protected:
	UPROPERTY(BlueprintReadOnly)
	FVector2D BackupInputDirection = FVector2D::Zero();

	UPROPERTY(EditDefaultsOnly)
	FVector2D BackupInputBounds = FVector2D(11.0f, 11.0f);

	UPROPERTY(EditDefaultsOnly)
	float BackupInputDeadzone = 4.0f;

	UPROPERTY(BlueprintAssignable)
	FOnBackupInputPressed OnBackupInputPressed;

	UPROPERTY(BlueprintAssignable)
	FOnBackupInputReleased OnBackupInputReleased;

	bool bSelectingBackupOption = false;

	void HandleDialogueBackupInput();

	void HandleCalloutBackupInput();

	TObjectPtr<UBackupCalloutWidget> BackupCalloutWidget = nullptr;

	TObjectPtr<UDialogueWidget> DialogueWidget = nullptr;

	EBackupInputState InputState;
		
};
