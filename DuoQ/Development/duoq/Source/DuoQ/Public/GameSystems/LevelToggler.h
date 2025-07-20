// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameSystems/Progression/TriggerTarget.h"
#include "LevelToggler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelToggled);

UCLASS()
class DUOQ_API ALevelToggler : public ATriggerTarget
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelToggler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	TArray<FName> LevelsToLoad;
	UPROPERTY(EditAnywhere)
	TArray<FName> LevelsToUnload;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLevelToggled OnToggled;

public:
	virtual void OnUnTrigger_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void LoadSingleLevel(FName name, FString str);

protected:
	FTimerHandle timer;
	int			 NumTasks = 0;
	int			 NumCompleted = 0;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void  CompleteTask();
	FName AppendName(FName name, FString str);

	void UnloadSingleLevel(FName name, FString str);
};
