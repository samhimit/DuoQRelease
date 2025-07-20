// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainMenuPawn.generated.h"

struct FInputActionValue;
class UCameraComponent;

UCLASS()
class DUOQ_API AMainMenuPawn : public APawn
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BackupInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ControlBackupInputAction;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	// Sets default values for this pawn's properties
	AMainMenuPawn();

	UFUNCTION(BlueprintCallable)
	void UnlockMicInput();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Begins recording and speech processing */
	void UnMute();

	/** Ends recording, flushes audio to be processed */
	void Mute();

	/** Toggles Between Recording and not recording and flushing */
	void ToggleMute();

	virtual void ControlOptions(const FInputActionValue& Value);
	
	void BackupInputStart();

	void BackupInputRelease();

	/*
	 * @param bToggle Whether to put push to talk in toggle or held mode
	 */
	UFUNCTION(BlueprintCallable)
	void SetupPushToTalkToggle(bool bToggle);

	// Event bindings for mute unmute
	TArray<uint32> TalkBindings;

	UPROPERTY()
	TObjectPtr<class UBackupInputComponent> BackupInputComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};
