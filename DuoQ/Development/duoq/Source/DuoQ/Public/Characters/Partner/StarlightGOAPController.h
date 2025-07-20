// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPController.h"
#include "StarlightGOAPController.generated.h"

class UStarlightSeeComponent;
/**
 *
 */
UCLASS()
class DUOQ_API AStarlightGOAPController : public AGOAPController
{
	GENERATED_BODY()

public:
	AStarlightGOAPController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// debug function that prints out desired world elements
	UFUNCTION(BlueprintCallable)
	void PrinDesiredWorld();

	/**
	 *	this function takes in a keyed goal to find the given action
	 *	@param KeyedGoalName - The name of the keyed goal
	 *	@param ActionName - which action from the keyed goal do you want to search for
	 *	@param NewCost - the new cost of this action
	 */
	UFUNCTION(BlueprintCallable)
	void SetCostOfTargetKeyedAction(FString KeyedGoalName, FString ActionName, float NewCost);

	/** checks if there's only 1 grunt enemy left. If that is the case, Starlight should directly tell the player to kill that grunt*/
	UFUNCTION(BlueprintCallable)
	void CheckNumberGrunt();

	/** checks the distance between Starlight and player, if the player is too far, Starlight will prioritize following the player
	 *   will return false if starlight is following and true if starlight is leading
	 */
	UFUNCTION(BlueprintCallable)
	bool CheckDistanceWithPlayer();

	// sets the parameter atom value to false
	UFUNCTION(BlueprintCallable)
	void ResetTargetAtomToFalse(FAtom Atom);

	// debug function that prints out current world elements
	UFUNCTION(BlueprintCallable)
	void PrintCurrentWorld();

	// called when an enemy is perceived
	UFUNCTION(BlueprintCallable)
	void HandleOnPerceiveEnemy(class UHealthComponent* HealthComp, class UDynamicGOAPComponent* GOAPComp, bool bIsInSight, bool bHasBeenSeen);

	UFUNCTION(BlueprintCallable)
	void HandleOnPerceive(class UHealthComponent* HealthComp, class UDynamicGOAPComponent* GOAPComp, bool bIsInsight, bool bHasBeenSeen);

	UFUNCTION(BlueprintCallable)
	void PrintDesiredWorldState();

	UFUNCTION(BlueprintCallable)
	void JoinWorldStateAfterAction();

	UFUNCTION(BlueprintCallable)
	void ClearStoredAction();

	// this integer decides how many plans starlight can generate at a time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GOAP)
	int NumPlan = 3;

	/** this checks an action is after a player prompt or not*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPrompted = false;

	// UFUNCTION(BlueprintCallable)
	TMap<FAtom, int>& GetGoals();

	// getting the GOAPPlanner of this GOAPController
	GOAPPlanner* GetGOAPPlanner() { return planner; }

	// priority goap generate plan that generates plan in the order of priority
	virtual bool generatePlan() override;

	UFUNCTION(BlueprintCallable, Category = GOAPController)
	UGOAPAction* AddKeyedActionToGOAPPlanner(FAtom Goal, const UClass* Class, FName ActionName, TArray<FAtom> Effects, TArray<FAtom> Preconditions, AActor* Target);

	/** record all the grunt enemies that are insight or attacking the player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> GruntEnemiesInsight;

	/** the number of grunt enemies that are currently in sight*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GruntEenemyInSight;

	/** whether or not starlight is leading the player - Yes: will wait for player, No: will catch up to player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsLeading = false;

	// the most prioritized goals will be at the start of the array
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GOAP)
	TArray<FAtom> mPriorityGoals;

	/** Grab the desired world atom according to the input and change to the new priority
	 *   @Param Goal - which goal do you want to change
	 *	@Param NewPriority - the new priority
	 *	@Param DGOAP - if the object has a dynamicgoap component, pass it in, otherwise pass in nullptr
	 */

	UFUNCTION(BlueprintCallable)
	void SetDesiredWorldAtomPriority(FAtom Goal, int NewPriority);

	/** Updating the desired world name with the given truth value: WILL ONLY WORK ON EXISTING WORLD STATES IN DESIRED WORLD!
	 * @Param Goal - the goale you want to change
	 */
	UFUNCTION(BlueprintCallable)
	void SetDesiredWorldState(FAtom Goal);

	UFUNCTION(BlueprintCallable)
	void TerminateCurrentAction();

	/** remove all keyed goals and actions currenty in Starlight's planner*/
	UFUNCTION(BlueprintCallable)
	void RemoveAllKeyedGoalAndAction();

	/** remove all keyed action and goals with the given keyname
	 *	@param KeyName - the genericn name of the action we want to remove
	 */
	UFUNCTION(BlueprintCallable)
	void RemoveAllKeyedActionWithName(FString KeyName);

	/** remove the generic action with the given keyname
	*   @param KeyName - the name of the action we want to remove
	*/
	UFUNCTION(BlueprintCallable)
	void RemoveActionWithName(FString KeyName);

	UFUNCTION(BlueprintCallable)
	void SetTargetActionCost(UGOAPAction* Action, int NewCost);

	UFUNCTION(BlueprintCallable)
	UGOAPAction* GetCurrentAction();

	UFUNCTION(BlueprintCallable)
	const UGOAPAction* GetNextAction();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool CurrentWorldContainsState(TArray<FAtom> WorldState);

	UFUNCTION(BlueprintCallable)
	TArray<UGOAPAction*> GetCurrentPlan();

	UFUNCTION(BlueprintCallable)
	void SetTargetEnemy(class AActor* Enemy) { TargetEnemy = Enemy; }

	UFUNCTION(BlueprintCallable)
	class AActor* GetTargetEnemy() { return TargetEnemy; }

	UFUNCTION(BlueprintCallable)
	void ClearTargetEnemy() { TargetEnemy = nullptr; }

	UFUNCTION(BlueprintCallable)
	int GetNumRemainingPlans();

	UFUNCTION(BlueprintCallable)
	void CheckGoalCompletion();

	UFUNCTION(BlueprintCallable)
	void AddToVisibleActorsWithTags(AActor* ActorToAdd) { VisibleActorsWithTags.AddUnique(ActorToAdd); }

	UFUNCTION(BlueprintCallable)
	void ClearVisibleActorsWithTags() { VisibleActorsWithTags.Empty(); }

	UFUNCTION(BlueprintCallable)
	void RemoveActorFromVisibleActors(AActor* ActorToRemove);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> VisibleActorsWithTags;

	/** should contain actors not already in visible actors with tags but still want to be registered*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsWithTags;

	/** this list copies the elements in visible actor with tags array when Player input is generated. It stays unchanged until
	 *   the LLM API is completed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> CachedVisibleActors;

	/** copy everything in VisibleActorsWithTags to CachedVisibleActors*/
	UFUNCTION(BlueprintCallable)
	void CacheVisibleActors();

	/** this function returns an FString formulated with all the visible tagged actors*/
	UFUNCTION(BlueprintCallable)
	FString GetTagOutputString();

	void SetPlayerFocusedComponent();

	TObjectPtr<UStarlightSeeComponent> PlayerFocusedStarlightSeeComponent;

	/** the distance between starlight and player to make starlight follow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceToFollow = 500.0f;

	/** for debug, caches the currently highlighted enemy based on starlight's action*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> CurrentHighlightedEnemy = nullptr;

	UPROPERTY(BlueprintReadWrite)
	class AActor* GOAPContextTarget = nullptr;

	UPROPERTY(BlueprintReadWrite)
	bool bIsPlayerNear = true;

	/** whether or not starlight should register the enemy from any source*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouRegisterEnemy = true;

	/** whether or not starlight should warn about distance*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	bool bShouldWarnDistance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float WarnDistanceTimer = 5.0f;

private:
	// contains the current goal the plan should reach, will only contain one goal at a time
	TArray<FAtom> mCurrentGoal;
	// the list of plans generated, will execute the plan at the front of the array first

	TArray<TArray<UGOAPAction*>> mStoredPlans;

	FTimerHandle DistanceHandle;

	UPROPERTY()
	UGOAPAction* CurrentAction;

	UPROPERTY()
	class AActor* TargetEnemy = nullptr;

protected:
	void ResetDistanceTimer();
	void debugInfo() override;
};
