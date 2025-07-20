// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "PriorityGOAPAction.generated.h"

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class DUOQ_API UPriorityGOAPAction : public UGOAPAction
{
	GENERATED_BODY()

public:
	// contructor
	UPriorityGOAPAction();
	// overriding the getworld function
	virtual UWorld* GetWorld() const override;

	virtual float getCost() override;

	/** This function sets the blackboard value "ShouldFinishExecute" to true to tell the BT we can proceed with the next action*/
	UFUNCTION(BlueprintCallable)
	void SetFinishExecute(bool NewValue);

	/** this function sets the priority of the keyed goal linked to this action*/
	UFUNCTION(BlueprintCallable)
	void SetKeyedGoalPriority(int NewPrio, FString ActionName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AActor* TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WarnTimer = 3.0f;

	/** keeps track of how many times Starlight warned the player to do something*/
	UPROPERTY(BlueprintReadWrite)
	int NumTimeWarned = 0;

	/** provide functionality for if we don't want the priority to be reset*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldResetPrio = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldWarn = true;

	UFUNCTION(BlueprintCallable)
	void SetTargetEnemy(class ADuoQCharacter* Target);
	UFUNCTION(BlueprintCallable)
	void UpdateKeyedAtomStatus(FString AtomName, bool NewState);
	UFUNCTION(BlueprintCallable)
	void ResetWarnTimer();
	UFUNCTION(BlueprintCallable)
	class AStarlightCharacter* GetStarlightCharacter();
	UFUNCTION(BlueprintCallable)
	class ADuoQPlayerCharacter* GetPlayer();

	UWorld* mWorld;

	UPROPERTY()
	TArray<FAtom> KeyedGoals;

protected:
	bool bShouldSetTimer = true;

private:
};
