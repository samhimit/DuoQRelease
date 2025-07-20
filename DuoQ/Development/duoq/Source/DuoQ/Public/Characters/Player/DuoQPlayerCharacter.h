// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Characters/DuoQCharacter.h"
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "DuoQPlayerCharacter.generated.h"

class UBackupInputComponent;
class UDialogueOptionWidget;
class UDuoQHUD;
class UReviveManager;
/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerReviveDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeathDelegate);

UENUM()
enum EInputState : uint8
{
	RegularGameplay,
	BackupInput
};

UENUM(BlueprintType)
enum class EPlayerEncounter : uint8
{
	UnShieldedGrunt,
	FirstShielded,
	Observatory,
	Turret1,
	Turret2
};

UCLASS()
class DUOQ_API ADuoQPlayerCharacter : public ADuoQCharacter
{
	GENERATED_BODY()
	UFUNCTION()
	void ClearTimer(UWorld* world, bool tick, bool deltaTime);

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCaptureComponent2D;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraRotationPoint;

	/** Sprint audio loop */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* SprintLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* ReviveSound;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CancelSprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReviveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BackupInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ViewControlsAction;

	UPROPERTY()
	TObjectPtr<UBackupInputComponent> BackupInputComponent;

public:
	ADuoQPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	EInputState InputState = EInputState::RegularGameplay;

public:
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** used to detect last edge so we can do leading edge or trailing edge*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLastEdge = false;

	/** used to determine whether player is at level 1 or 2*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAtLevel2 = false;

	/** did they die from grunt or turret?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDeathByGrunt = true;

	/** Debug sound action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DebugSoundAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** if this is true there will be an outline for starlight's target enemy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ToggleStarlightTarget = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	float WalkSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	float SprintSpeed = 650.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float SprintDot = 0.6f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StarlightSprint)
	float SprintTowardsStarlightMultiplier = 1.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StarlightSprint)
	float DotActivateStarlightSprint = 0.6f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StarlightSprint)
	bool IsStarlightSprintActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StarlightSprint)
	float NormalFOV = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StarlightSprint)
	float StarlightSprintFOV = 110.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StarlightSprint)
	float FOVChangeSpeed = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StarlightSprint)
	float ReviveHoldTime = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StarlightSprint)
	float AutoReviveHoldTime = 10.0f;
	UPROPERTY(VisibleANywhere, BlueprintReadWrite)
	FTimerHandle GetGunHandle;

	/** keep track of the last PP player stepped on*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* CurrentPP = nullptr;

	UPROPERTY()
	FVector ReviveLocation;

	/** tracks which encounter player is in*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerEncounter CurrentEncounter = EPlayerEncounter::UnShieldedGrunt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StarlightSprint)
	AActor* Starlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	USoundBase* SprintStartSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	USoundBase* StarlightSprintStartSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	float SprintLoopFadeTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	class UDuoQElectricRevolver* GunComponent;
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void GetGun();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void GetGunFromPickUp();

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void SaveCheckSprint(bool input) { CheckSprintResult = input; }
	UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
	void OnGunPickUp();
	UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
	void FirstOnGunPickUp();

	UFUNCTION(BlueprintCallable, Category = Health)
	void HandleDeath() override;

	UFUNCTION(BlueprintCallable, Category = GameOver)
	void HandleGameOver();
	UFUNCTION(BlueprintCallable, Category = Spawn)
	void TeleportToSpawn();

	virtual void Tick(float DeltaSeconds) override;

	// Object to drop on death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
	TSubclassOf<AActor> Soul;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Revive")
	float MaxReviveDist = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TObjectPtr<class UDuoQHUD> PlayerHUD;

	UPROPERTY(BlueprintAssignable, Category = "Revive")
	FOnPlayerReviveDelegate OnPlayerRevive;

	UPROPERTY(BlueprintAssignable, Category = "Revive")
	FOnPlayerDeathDelegate OnPlayerDeath;

	void DisableMovementOnDeath();
	void RevivePlayer();

	UFUNCTION(BlueprintCallable)
	void RemoveActivatedTrigger(FString Dialogue);

protected:
	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value);

	/** Called for looking input */
	virtual void Look(const FInputActionValue& Value);

	void BackupInputStart();

	void BackupInputRelease();

	/** Begins recording and speech processing */
	void UnMute();

	/** Ends recording, flushes audio to be processed */
	void Mute();

	/** Toggles Between Recording and not recording and flushing */
	void ToggleMute();

	void ToggleSprint();
	void StartSprint();

	void TickSprint();

	/** Toggles pause menu **/
	void Pause();

	void ShowControls();

	void HideControls();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/*
	 * @param bToggle Whether to put push to talk in toggle or held mode
	 */
	UFUNCTION(BlueprintCallable)
	void SetupPushToTalkToggle(bool bToggle);

	UPROPERTY(BlueprintReadOnly)
	// Sensitivity for the mouse input for pitch and yaw.
	float LookSensitivity = 1.0f;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	USceneCaptureComponent2D* GetSceneCaptureComponent() const { return SceneCaptureComponent2D;}

	void SetSensitivity(float sensitivity) { LookSensitivity = sensitivity; }

	UFUNCTION()
	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SpawnedSoul;

private:
	bool CheckSprintResult = false;

	// Event bindings for mute unmute
	TArray<uint32> TalkBindings;

	UPROPERTY()
	FTimerHandle ReviveTimerHandle;

	UPROPERTY()
	FTimerHandle AutoReviveTimerHandle;

	UPROPERTY()
	FTimerHandle GunPickupTimerHandle;

	UFUNCTION()
	void StartReviveAttempt();

	UFUNCTION()
	void CancelReviveAttempt();

	UFUNCTION()
	void CompleteRevive();
};
