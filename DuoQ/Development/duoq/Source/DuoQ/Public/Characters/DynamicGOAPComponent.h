// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "DynamicGOAPComponent.generated.h"

class UPriorityGOAPAction;

USTRUCT(BlueprintType, Blueprintable)
struct FActionMaker
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAtom> Preconditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAtom> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPriorityGOAPAction> ActionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NameToAdd;
};

USTRUCT(BlueprintType, Blueprintable)
struct FDynamicGOAPPreset
{
	GENERATED_USTRUCT_BODY()

	/** The goal that will be reached after destroying/Completing the owner of the component*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DynamicGOAP)
	FAtom KeyedGoal;

	/** The FAtoms that needs to be added into the world. For example, for an enemy, the atoms is (InSight, true/false) and (Alive, true/false) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DynamicGOAP)
	TArray<FAtom> AtomsToAdd;

	/** The initial priority linked to the goal*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DynamicGOAP)
	int DefaultPriority;

	/** holds an array of FActionMaker*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DynamicGOAP)
	TArray<FActionMaker> ActionsToMake;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUOQ_API UDynamicGOAPComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDynamicGOAPComponent();

	/** takes the settings struct and create specified goals and actions accordingly*/
	UFUNCTION(BlueprintCallable)
	void SetGoalAndCreateActions(FDynamicGOAPPreset Setting);

	UFUNCTION(BlueprintCallable)
	void CreateAllGoalAndActions();

	/** Function used to removed all the keyed actions linked to the owner of this component */
	UFUNCTION(BlueprintCallable)
	void RemoveLinkedActions(AActor* Actor, FString Name, bool Status);

	/** update current world with the specific atom name*/
	UFUNCTION(BlueprintCallable)
	void UpdateKeyedAtomStatus(FString AtomName, bool NewState);

	/** holds the information needed to create goals and actions for the owner of this component*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DynamicGOAP)
	TArray<FDynamicGOAPPreset> Settings;

	/** sets the priority of the linked goal according to the atom name provided*/
	UFUNCTION(BlueprintCallable)
	void SetPrioWithSpecificAtom(FString AtomName, int NewPrio);

	/** whether or not Starlight's sight should autimatically register this dynamic goap action when AIPerception is updated*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DynamicGOAP)
	bool bShouldRegisterWithStarlightSight = true;

	UPROPERTY(BlueprintReadOnly)
	FString AliveStateName;

	UPROPERTY(BlueprintReadOnly)
	FString InSightStateName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBeenSeen = false;

	/** records the action names created by this dynamic GOAP Component*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> AtomNames;

	/** returns the atom name specific to the owner of this component. Returns empty string if the atom is not found*/
	UFUNCTION(BlueprintCallable)
	FString GetSpecificAtomName(FString KeyWord);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FString NameID;
	FAtom	KillGoal;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
