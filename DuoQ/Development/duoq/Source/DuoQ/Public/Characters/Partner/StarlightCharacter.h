// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Characters/DuoQCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Characters/Abilities/DuoQStarlightWeapon.h"
#include "Animation/AnimInstance.h"
#include "Components/TimelineComponent.h"
#include "EnhancedInputComponent.h"
#include "StarlightCharacter.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FAreaPreset
{
	GENERATED_USTRUCT_BODY()

	/** where the area is*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AreaEntrance;

	/** what starlight will say when moving to this area*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* CallOutLine;

	/** the subtitle of the voice line*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Subtitle;
};

USTRUCT(BlueprintType, Blueprintable)
struct FCheckpointObject
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<AActor>> Targets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int							   Progress;
};

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStarlightReviveDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStarlightDeadDelegate);

UCLASS()
class DUOQ_API AStarlightCharacter : public ADuoQCharacter
{
	GENERATED_BODY()

public:
	AStarlightCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	FVector GetPlayerSpawnLocation();

	UFUNCTION(BlueprintCallable, Category = Health)
	void HandleDeath() override;

	UFUNCTION(BlueprintCallable, Category = GameOver)
	void HandleGameOver();

	/** this function is used when starlight tries to find something when her mouse is dead
	 * @param TargetToFind - the target starlight tries to find without control rotation
	 */
	UFUNCTION(BlueprintCallable)
	bool StrafeWhileMouseDead(AActor* TargetToFind);

	/** this function tells whether the player is in front of starlight
	 *	@param InFrontAngle - the absolute angle by which player is considered in front of starlight
	 */
	UFUNCTION(BlueprintCallable)
	bool IsPlayerInFront(float InFrontAngle);

	/** Finds the closest pressure plate in the scene*/
	UFUNCTION(BlueprintCallable)
	FVector FindClosestPressurePlate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UDuoQStarlightWeapon> Gun3P;

	/** trackes whether or not starlight is blocked by a door*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPlayerBlocked = true;

	/** primarily meant as a fail save where if we definitely don't want starlight to follow player we can */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanFollowPlayer = true;

	/** Starlight will not respond to anything the player says if this bool is set to true*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnlyWaitPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bForceMove = false;

	/** whether or not Starlight will keep the focal point and not change*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldKeepFocus = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldShoot = true;

	/** this is the look offset of Tala when she looks at the player during stayput*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> LookOffsets;

	/** this is the longest Tala will look at a direction of the player before changing a focus*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookDurationUpperBound = 7.0f;

	/** this is the shortst Tala will look at a dircetion*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookDurationLowerBound = 4.0f;

	UPROPERTY()
	float TargetLookDurationCounter = 5.0f;

	UPROPERTY()
	float CurrentLookDurationCounter = 0.0f;

	/** whether or not starlight is pushable when player hits her*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPushable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UAudioComponent> VoiceSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	UAudioComponent* ReviveSound;

	/** this list stores the fountains that starlight wants to jump on top of*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<AActor>> FountainTargets;

	/** this list stores the list of objects to destroy when loading*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCheckpointObject> CheckpointDestroyList;

	/** which fountain Starlight is on rn*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrFountainIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasWeapon = false;

	/** returns the target location of the fountain target at index 0*/
	UFUNCTION(BlueprintCallable)
	FVector GetCurrentFountainTargetLocation();
	UFUNCTION(BlueprintCallable)
	void DestroyObjects();

	FTransform DebugTransform;

	UFUNCTION(BlueprintCallable)
	FTransform ReturnDebugTrans() { return DebugTransform; }

	/** used to track something for mouse broken sequence*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasSetDirection = false;

	/** the pressure plate Starlight is standing on*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* CurrentPressurePlate = nullptr;

	/** if this is true, Starlight should not turn her camear at all*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMouseDead = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> ShootMontage;

	UPROPERTY()
	FTimeline MyTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* FloatCurve;

	UFUNCTION()
	void TimelineCallback(float val);

	UFUNCTION(BlueprintImplementableEvent)
	void TimelineFinishedCallback();
	UFUNCTION(BlueprintCallable, Category = "Tick")
	void TimelineTick(float deltaTime);

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

	UFUNCTION(BlueprintCallable)
	void StartShooting();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void GetGun();

	UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
	void OnGunPickUp();

	UFUNCTION(BlueprintImplementableEvent)
	TArray<AActor*> GrabVisibleActors();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PingStarlight(FVector spawnTransformLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* CurrentTargetEnemy = nullptr;

	/** record the area in level, remove an area once it is beaten*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Areas)
	TArray<FAreaPreset> AreasInLevel;

	/** where starlight and player should return to when finished a room*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Areas)
	FVector HubLocation;

	/** whether or not starlight and the player is at the main hub*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Areas)
	bool bIsAtHub = true;

	/** the index of the current area*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Areas)
	int CurrentArea;

	/** how many areas did the players beat*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumAreaBeat;

	UFUNCTION()
	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void TurnToPlayer();

	UFUNCTION(BlueprintCallable)
	void TurnToFuturePlayer();

	UFUNCTION()
	void ReviveStarlight();

	UFUNCTION()
	UCameraComponent* GetFollowCamera() const;

	UPROPERTY(BlueprintAssignable, Category = "Revive")
	FOnStarlightReviveDelegate OnStarlightRevive;

	UPROPERTY(BlueprintAssignable, Category = "Revive")
	FOnStarlightDeadDelegate OnStarlightDead;

	UPROPERTY()
	FVector ReviveLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CameraOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StarlightSprint)
	float ReviveHoldTime = 3.0f;

	TArray<AActor*> DamagedEnemies;

	// Object to drop on death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
	TSubclassOf<AActor> Soul;

	UFUNCTION(BlueprintCallable)
	void StartReviveAttempt();

	UFUNCTION(BlueprintCallable)
	bool CancelReviveAttempt();

	UFUNCTION(BlueprintCallable)
	void CompleteRevive();

	UFUNCTION()
	void UpdateHealthBar(float NewHealth, float MaxHealth);

protected:
	UFUNCTION()
	virtual void BeginPlay() override;

	UPROPERTY()
	FRotator TurnTarget;

	UPROPERTY()
	FRotator TurnOrigin;

	/** how long Tala has been looking at the player so we know we should change her LOS*/
	UPROPERTY()
	float LookCounter = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

private:
	UPROPERTY()
	AActor* SpawnedSoul;

	bool bIsGoingRight = true;

	UPROPERTY()
	FTimerHandle RevivePlayerTimerHandle;
};
