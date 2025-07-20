// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"
#include "Engine/TriggerBox.h"
#include "GameSystems/DuoQGameMode.h"
#include "GameFramework/Actor.h"
#include "ACombatManager.generated.h"

class ATriggerTarget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatManagerActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGruntDeath, int, Remaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatManagerDeactivated);

UENUM(BlueprintType)
enum class EGruntState : uint8
{
	Standby	  UMETA(DisplayName = "Standby"),
	Attacking UMETA(DisplayName = "Attacking"),
	NotFound  UMETA(DisplayName = "Not Found")
};

UCLASS()
class DUOQ_API ACombatManager : public AActor
{
	GENERATED_BODY()

public:
	ACombatManager();

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* StartOfCombat;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	TSoftObjectPtr<ATriggerTarget> StartCombatEarlyTrigger;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<AGruntShotgunEnemy*> Grunts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 NumberOfAttackingGrunts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<AGruntShotgunEnemy*> AttackingGrunts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<AGruntShotgunEnemy*> StandbyGrunts;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCombatManagerActivated OnCombatManagerActivated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCombatManagerDeactivated OnCombatManagerDeactivated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGruntDeath OnGruntDeath;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	EGruntState GetGruntState(AGruntShotgunEnemy* Grunt) const;

	UFUNCTION()
	void HandleGruntDeath();

	UFUNCTION()
	void HandleGruntHit(AGruntShotgunEnemy* Grunt, float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateGruntStates();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateGrunts();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetAllGruntsToStandby();

	UFUNCTION(BlueprintCallable, Category = "Combat Manager")
	void AddGrunts(const TArray<AGruntShotgunEnemy*>& NewGrunts);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float IdleTimerLength = 10.0f;

protected:
	UPROPERTY(EditAnywhere, Category = "Dialogue", BlueprintReadWrite)
	TObjectPtr<class UDialogueTriggerComponent> DialogueTriggerComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TSubclassOf<class UMusicManager> CombatMusicClass;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TSubclassOf<class UMusicManager> PostCombatMusicClass;

private:
	// Private variable for the tick interval
	UPROPERTY(EditDefaultsOnly, Category = "Combat Manager")
	float TickInterval;

	// Timer to track elapsed time
	float TimeSinceLastUpdate;

	// Update the attacking and standby grunts
	void UpdateClosestGrunts();

	UPROPERTY()
	bool activated = false;

	UPROPERTY()
	bool bIsComplete = false;

	UPROPERTY()
	ADuoQGameMode* DuoQGameMode;

	FTimerHandle IdleDialogueHandle;

	UFUNCTION()
	void FireHintDialogue();

};
