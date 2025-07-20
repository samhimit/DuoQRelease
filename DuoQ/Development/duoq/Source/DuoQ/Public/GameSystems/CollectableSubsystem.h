// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CollectableSubsystem.generated.h"

/**
 *
 */
UCLASS(Blueprintable, Abstract)
class DUOQ_API UCollectableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

public:
	/** the list of cllectables that are in the world*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ADuoQCollectables*> Collectables;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalCollectableCounts = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, bool> CollectedItems;

	UFUNCTION(BlueprintCallable)
	void AddToCollectedItems(FName Name, bool Value);

	UFUNCTION(BlueprintCallable)
	void AddToCollectables(class ADuoQCollectables* Added) { Collectables.AddUnique(Added); }

	UFUNCTION(BlueprintCallable)
	void RemoveFromCollectables(class ADuoQCollectables* Removed) { Collectables.Remove(Removed); }

	UFUNCTION(BlueprintCallable)
	void SaveCollectables();

	UPROPERTY(EditDefaultsOnly)
	FString AllCollectablesEvent = TEXT("AllCollectables");
};
