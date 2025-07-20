// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StarlightAffinityMeterSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSAMUpdate,int,ScoreChange,bool,bChangedLevels);

UENUM(BlueprintType)
enum class EStarlightAffectionLevel : uint8
{
	Abysmal,
	Fair,
	Good
};

/**
 *
 */
UCLASS(Blueprintable, Abstract)
class DUOQ_API UStarlightAffinityMeterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

public:
	/** prints updates to SAM according to whether the meter increased or decreased*/
	UFUNCTION(BlueprintCallable)
	FString PrintSAMUpdateOnScreen(bool Added);

	/** prints the important info from SAM and */
	UFUNCTION(BlueprintCallable)
	FString PrintSAMInfoAsString();

	UFUNCTION(BlueprintCallable)
	void UpdateSAMScore(int ScoreChange);

	/** what is the current SAM*/
	UPROPERTY(BlueprintReadOnly)
	EStarlightAffectionLevel CurrentSAMLevel = EStarlightAffectionLevel::Fair;

	UPROPERTY(BlueprintAssignable)
	FOnSAMUpdate OnSAMUpdate;

protected:
	/** What is the meximum Starlight Affnity Level*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EStarlightAffectionLevel StartingSAMLevel = EStarlightAffectionLevel::Fair;

	/** the score needed to reach the next level of SAM*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EStarlightAffectionLevel,int> ScoreToLevelUp;

	/** current score of the player, will reset everytime the player levels up*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int CurrentScore = 0;
};
