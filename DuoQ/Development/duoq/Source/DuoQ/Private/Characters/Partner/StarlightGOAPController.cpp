// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Partner/StarlightGOAPController.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "GameSystems/Communication/CommunicationSubsystem.h"
#include "Engine/GameEngine.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/HealthComponent.h"
#include "Characters/Enemy/GruntShotgunEnemy.h"
#include "Characters/DynamicGOAPComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/Partner/StarlightSeeComponent.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "CoreGlobals.h"
#include "GameSystems/DuoQGameInstance.h"

AStarlightGOAPController::AStarlightGOAPController(const FObjectInitializer& ObjectInitializer)
	: AGOAPController(ObjectInitializer)
{
	mPriorityGoals.Empty();
	mStoredPlans.Empty();
}

void AStarlightGOAPController::PrinDesiredWorld()
{
	for (const TPair<FAtom, int>& pair : desiredWorld)
	{
		FString Name = pair.Key.name + " " + FString::FromInt(pair.Key.value);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Name);
		}
	}
}

void AStarlightGOAPController::SetCostOfTargetKeyedAction(FString KeyedGoalName, FString ActionName, float NewCost)
{
	TArray<UGOAPAction*> KeyedActionsForGoal = planner->getKeyedActions(KeyedGoalName);
	for (UGOAPAction* Action : KeyedActionsForGoal)
	{
		if (Action->name.Contains(ActionName))
		{
			Action->cost = NewCost;
			Action->setCost(NewCost);
		}
	}
}

void AStarlightGOAPController::CheckNumberGrunt()
{
	// check if only 1 grunt enemy is left
	if (GruntEnemiesInsight.Num() == 1)
	{
		AActor* Grunt = GruntEnemiesInsight[0];
		if (AGruntShotgunEnemy* GSE = Cast<AGruntShotgunEnemy>(Grunt))
		{
			if (UDynamicGOAPComponent* DGOAP = GSE->GetComponentByClass<UDynamicGOAPComponent>())
			{
				FAtom Atom;
				Atom.name = DGOAP->GetSpecificAtomName(FString(TEXT("Kill")));
				Atom.value = true;

				// try to find the SLPrompt action
				// SetCostOfTargetKeyedAction(Atom.name, FString(TEXT("SLPrompt")), 10.0);
			}
		}
	}
}

bool AStarlightGOAPController::CheckDistanceWithPlayer()
{
	if (AStarlightCharacter* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld()))
	{
		if (UHealthComponent* Health = Starlight->GetComponentByClass<UHealthComponent>())
		{
			if (Health->IsDead())
			{
				return false;
			}
		}
	}

	if (ADuoQPlayerCharacter* Player = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(GetWorld()))
	{
		if (APawn* OwnerPawn = GetPawn())
		{
			float		  Distance = FVector::Distance(Player->GetActorLocation(), OwnerPawn->GetActorLocation());
			TArray<FAtom> Atoms;
			FAtom		  Nearby;
			Nearby.name = FString(TEXT("IsPlayerNearby"));

			if (!IsLeading)
			{
				FAtom Atom;
				Atom.name = FString(TEXT("ShouldFollowMe"));
				Atom.value = true;

				if (Distance > DistanceToFollow)
				{
					Nearby.value = false;
					Atoms.Emplace(Nearby);
					updateCurrentWorld(Atoms);
					bIsPlayerNear = false;

					SetDesiredWorldAtomPriority(Atom, 8);
				}
				else
				{
					Nearby.value = true;
					Atoms.Emplace(Nearby);
					updateCurrentWorld(Atoms);
					bIsPlayerNear = true;

					SetDesiredWorldAtomPriority(Atom, 12);
				}

				if (AStarlightCharacter* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this))
				{
					if (Starlight->bIsMouseDead)
					{
						// SetDesiredWorldAtomPriority(Atom, 12);
					}
				}
				return false;
			}
			else // starlight is taking charge
			{
				FAtom Atom;
				Atom.name = FString(TEXT("Stay"));
				Atom.value = true;

				if (Distance > DistanceToFollow)
				{
					Nearby.value = false;
					Atoms.Emplace(Nearby);
					updateCurrentWorld(Atoms);
					bIsPlayerNear = false;

					SetDesiredWorldAtomPriority(Atom, 7);

					// should warn about the distance
					if (bShouldWarnDistance)
					{
						bShouldWarnDistance = false;
						GetWorld()->GetTimerManager().SetTimer(DistanceHandle, this, &AStarlightGOAPController::ResetDistanceTimer, WarnDistanceTimer, false);
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					Nearby.value = true;
					Atoms.Emplace(Nearby);
					updateCurrentWorld(Atoms);
					bIsPlayerNear = true;
					SetDesiredWorldAtomPriority(Atom, 7);
				}
			}
		}
	}
	return false;
}

/** set the atom in desiredworld TMap to false */
void AStarlightGOAPController::ResetTargetAtomToFalse(FAtom Atom)
{
	int*	value = desiredWorld.Find(Atom);
	FString name = Atom.name;
	FAtom	newAtom = FAtom(name, false);
	desiredWorld.Remove(Atom);
	desiredWorld.Add(TTuple<FAtom, int>(newAtom, *value));
}

void AStarlightGOAPController::PrintCurrentWorld()
{
	TMap<FString, bool> Map = wsCurrentWorld.getAtoms();
	for (const auto& Atom : Map)
	{
		FString Name = Atom.Key + " " + FString::FromInt(Atom.Value);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Name);
		}
	}
}

void AStarlightGOAPController::HandleOnPerceiveEnemy(UHealthComponent* HealthComp, UDynamicGOAPComponent* GOAPComp, bool bIsInSight, bool bHasBeenSeen)
{
	// we only want to generate the action when it's the first time the enemy is seen
	if (!bHasBeenSeen)
	{
		// GOAPComp->SetUpWorldState();
		// GOAPComp->GenerateKillAction();
		// GOAPComp->GenerateMoveAction();
	}

	// see if we need to update current world or not
	if (HealthComp->GetCurrentHealth() > 0.0f)
	{
		TArray<FAtom> Update;
		Update.Emplace(GOAPComp->InSightStateName, bIsInSight);
		updateCurrentWorld(Update);
	}
}

void AStarlightGOAPController::HandleOnPerceive(UHealthComponent* HealthComp, UDynamicGOAPComponent* GOAPComp, bool bIsInsight, bool bHasBeenSeen)
{
	if (!bHasBeenSeen)
	{
		if (GOAPComp != nullptr)
		{
			GOAPComp->bHasBeenSeen = true;
			// GOAPComp->SetGoalAndCreateActions();
			GOAPComp->CreateAllGoalAndActions();

			// set cover goal priority only for shielded grunts
			if (HealthComp != nullptr)
			{
				if (HealthComp->bHasShield)
				{

					FAtom Atom;
					Atom.name = FString(TEXT("InCover"));
					Atom.value = true;
					SetDesiredWorldAtomPriority(Atom, 2);
				}
			}
		}
	}

	if (HealthComp != nullptr && GOAPComp != nullptr)
	{
		// see if we need to update current world or not
		if (HealthComp->GetCurrentHealth() > 0.0f)
		{
			FString		  InSightStateName = GOAPComp->GetOwner()->GetName() + TEXT("InSight");
			TArray<FAtom> Update;
			Update.Emplace(InSightStateName, bIsInsight);

			// also adjust the priority of move action based on whether the enemy is insight or not (don't need this anymore)
			/* FAtom Atom;
			Atom.name = GOAPComp->GetSpecificAtomName(FString(TEXT("Move")));
			Atom.value = true;

			if (bIsInsight)
			{
				SetDesiredWorldAtomPriority(Atom, 7);
			}
			else
			{
				SetDesiredWorldAtomPriority(Atom, 1);
			}*/

			updateCurrentWorld(Update);
		}
	}
}

void AStarlightGOAPController::PrintDesiredWorldState()
{
	TMap<FString, bool> Map = wsDesiredWorld.getAtoms();
	for (const auto& Atom : Map)
	{
		FString Name = Atom.Key + " " + FString::FromInt(Atom.Value);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Name);
		}
	}
}

void AStarlightGOAPController::JoinWorldStateAfterAction()
{
	if (CurrentAction != nullptr)
	{
		wsCurrentWorld.joinWorldState(CurrentAction->getEffects());
	}
}

void AStarlightGOAPController::ClearStoredAction()
{
	for (int i = 0; i < mStoredPlans.Num(); i++)
	{
		mStoredPlans[i].Empty();
	}
	mStoredPlans.Empty();
}

TMap<FAtom, int>& AStarlightGOAPController::GetGoals()
{
	return desiredWorld;
}

bool AStarlightGOAPController::generatePlan()
{
	mPriorityGoals.Empty();
	mStoredPlans.Empty();
	for (auto Pair : desiredWorld)
	{
		mPriorityGoals.Emplace(Pair.Key);
	}

	// we only want to generate a certain amount of plan at a time
	int NumTargetPlans = NumPlan;
	if (NumTargetPlans > mPriorityGoals.Num())
	{
		// this seems redundant, wouldn't the check to make sure there are goals left catch this case?
		NumTargetPlans = mPriorityGoals.Num();
	}

	// TODO we need to sort the priority goals array by priority
	TMap<FAtom, TArray<UGOAPAction*>> GeneratedPlans;
	TMap<FAtom, float>				  GeneratedPlanCosts;

	while (NumTargetPlans > 0 && mPriorityGoals.Num() > 0)
	{
		// gets the most important goal in the list
		mCurrentGoal.Push(mPriorityGoals[0]);
		setGoal(mCurrentGoal);
		planner->setGoal(&wsDesiredWorld);
		// if a plan is successfully generated, store that in the current plan list
		if (Super::generatePlan())
		{
			// mStoredPlans.Push(getPlan());
			//  find the cost of the plan
			float cost = 0.0f;
			for (UGOAPAction* Action : getPlan())
			{
				if (Action && Action->IsValidLowLevel())
				{
					cost += Action->cost;
				}
			}
			// store the weighted cost and plan
			GeneratedPlans.FindOrAdd(mPriorityGoals[0]) = getPlan();
			GeneratedPlanCosts.FindOrAdd(mPriorityGoals[0]) = cost * desiredWorld[mPriorityGoals[0]];

			mCurrentGoal.Empty();
		}
		// remove the goal at the start of priority goal list
		mPriorityGoals.RemoveAt(0);
		NumTargetPlans--;
	}

	// sort the plans based off weighted costs
	GeneratedPlans.KeySort([GeneratedPlanCosts](const FAtom& ip1, const FAtom& ip2) {
		return GeneratedPlanCosts[ip1] < GeneratedPlanCosts[ip2];
	});

	// place the ordered plans into the StoredPlans
	for (auto Plan : GeneratedPlans)
	{
		if (!Plan.Value.IsEmpty())
		{
			mStoredPlans.Emplace(Plan.Value);
		}
	}

	return false;
}

void AStarlightGOAPController::SetDesiredWorldAtomPriority(FAtom Goal, int NewPriority)
{
	if (desiredWorld.Contains(Goal))
	{
		desiredWorld[Goal] = NewPriority;
	}
}

void AStarlightGOAPController::SetDesiredWorldState(FAtom Goal)
{
	FAtom Inverse = FAtom(Goal.name, !Goal.value);
	if (desiredWorld.Contains(Inverse))
	{
		int Prio = desiredWorld[Inverse];
		desiredWorld.Remove(Inverse);
		desiredWorld.FindOrAdd(Goal) = Prio;
	}
}

void AStarlightGOAPController::TerminateCurrentAction()
{
	if (CurrentAction != nullptr)
	{
		if (UPriorityGOAPAction* Curr = Cast<UPriorityGOAPAction>(CurrentAction))
		{
			Curr->SetFinishExecute(true);
		}
	}
}

void AStarlightGOAPController::RemoveAllKeyedGoalAndAction()
{
	// get all the goals in the keyedatoms map
	TArray<FAtom> Atoms;
	KeyedAtoms.GetKeys(Atoms);

	for (FAtom Atom : Atoms)
	{
		desiredWorld.Remove(Atom);
		RemoveKeyedActionFromGOAPPlanner(Atom);
	}
}

void AStarlightGOAPController::RemoveAllKeyedActionWithName(FString KeyName)
{
	// get all the goals in the keyedatoms map
	TArray<FAtom> Atoms;
	KeyedAtoms.GetKeys(Atoms);

	for (FAtom Atom : Atoms)
	{
		if (Atom.name.Contains(KeyName))
		{
			desiredWorld.Remove(Atom);
			RemoveKeyedActionFromGOAPPlanner(Atom);
		}
	}
}

void AStarlightGOAPController::RemoveActionWithName(FString KeyName)
{
	FAtom TrueState = FAtom(KeyName, true);
	FAtom FalseState = FAtom(KeyName, false);
	if (desiredWorld.Contains(TrueState))
	{
		desiredWorld.Remove(TrueState);
	}
	else if (desiredWorld.Contains(FalseState))
	{
		desiredWorld.Remove(FalseState);
	}
}

void AStarlightGOAPController::SetTargetActionCost(UGOAPAction* Action, int NewCost)
{
}

// getter for Current Action
UGOAPAction* AStarlightGOAPController::GetCurrentAction()
{
	return CurrentAction;
}

// Get the next action to execute
const UGOAPAction* AStarlightGOAPController::GetNextAction()
{
	// set the currentaction to nullptr first
	CurrentAction = nullptr;
	// check if there are plans left, if there aren't we'll need to create a new plan (this will also happen the first time since we have no plans)
	if (mStoredPlans.Num() == 0)
	{
		generatePlan();
		// debugInfo();
	}

	if (mStoredPlans.Num() != 0)
	{
		// get the next action in the first stored plan, which is the plan currently with the highest prio
		if (mStoredPlans[0].Num() != 0)
		{
			CurrentAction = mStoredPlans[0][mStoredPlans[0].Num() - 1];
			if (CurrentAction->IsValidLowLevel())
			{
				// then we'll pop it off the list
				mStoredPlans[0].RemoveAt(mStoredPlans[0].Num() - 1);

				// then we'll want to make sure we can execute the action
				const bool bPredoncitionsAreMet = wsCurrentWorld.isIncluded(CurrentAction->getPreconditions());
				// Checks the procedural precondition of the action.
				const bool bProceduralPreconditionFulfilled = CurrentAction->checkProceduralPrecondition(GetPawn());
				if (!bPredoncitionsAreMet || !bProceduralPreconditionFulfilled)
				{
					CurrentAction = nullptr;
				}
			}
			return CurrentAction;
		}
		else
		{
			generatePlan();
			// mStoredPlans.RemoveAt(0);
		}
	}
	return nullptr;
}

void AStarlightGOAPController::BeginPlay()
{
	Super::BeginPlay();

	// setting up crowd following component
	if (UCrowdFollowingComponent* CFC = FindComponentByClass<UCrowdFollowingComponent>())
	{
		// CFC->SetCrowdSeparation(true);
		// CFC->SetCrowdSeparationWeight(50.0f);
		// CFC->SetCrowdAvoidanceRangeMultiplier(1.1f);
	}

	// set up world context for the actions
	for (UGOAPAction* action : auxActions)
	{
		if (UPriorityGOAPAction* prio = Cast<UPriorityGOAPAction>(action))
		{
			prio->mWorld = GetWorld();
		}
	}
	// set up Communication subsystem's planner
	if (UDuoQGameInstance* game = Cast<UDuoQGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (UCommunicationSubsystem* CommunicationSubsystem = game->GetSubsystem<UCommunicationSubsystem>())
		{
			CommunicationSubsystem->SetGOAPPlanner(planner);
		}
	}
}

bool AStarlightGOAPController::CurrentWorldContainsState(TArray<FAtom> WorldState)
{
	GOAPWorldState desiredWorldState = GOAPWorldState();
	desiredWorldState.cleanAtoms();

	for (FAtom atom : WorldState)
		desiredWorldState.addAtom(atom.name, atom.value);

	return wsCurrentWorld.isIncluded(desiredWorldState);
}

TArray<UGOAPAction*> AStarlightGOAPController::GetCurrentPlan()
{
	if (!mStoredPlans.IsEmpty())
	{
		return mStoredPlans[0];
	}
	return TArray<UGOAPAction*>();
}

int AStarlightGOAPController::GetNumRemainingPlans()
{
	return mStoredPlans.Num();
}

void AStarlightGOAPController::CheckGoalCompletion()
{
	// get all the goals in the keyedatoms map
	TArray<FAtom> Atoms;
	KeyedAtoms.GetKeys(Atoms);
	// loop through the array to check if the conditions are met already
	for (FAtom Atom : Atoms)
	{
		TArray<FAtom> Goal = { Atom };
		if (CurrentWorldContainsState(Goal))
		{
			desiredWorld.Remove(Atom);
			RemoveKeyedActionFromGOAPPlanner(Atom);
		}
	}

	// reset the plans array if the most prioritized plan is finished
	if (mStoredPlans.IsEmpty() || mStoredPlans[0].IsEmpty())
	{
		mStoredPlans.Empty();
	}
}

void AStarlightGOAPController::RemoveActorFromVisibleActors(AActor* ActorToRemove)
{
	// see if this actor is in the list
	if (VisibleActorsWithTags.Find(ActorToRemove) != INDEX_NONE)
	{
		VisibleActorsWithTags.Remove(ActorToRemove);
	}
}

void AStarlightGOAPController::CacheVisibleActors()
{
	CachedVisibleActors.Empty();
	for (AActor* Actor : VisibleActorsWithTags)
	{
		AActor* CacheActor = Actor;
		CachedVisibleActors.AddUnique(CacheActor);
	}
}

FString AStarlightGOAPController::GetTagOutputString()
{
	// populate the Cache array here
	CacheVisibleActors();
	FString Output;

	for (int i = 0; i < CachedVisibleActors.Num(); i++)
	{
		if (CachedVisibleActors[i] != nullptr)
		{
			// add index
			Output = Output + CachedVisibleActors[i]->GetName();

			if (UStarlightSeeComponent* See = CachedVisibleActors[i]->GetComponentByClass<UStarlightSeeComponent>())
			{
				for (FString Tag : See->Tags)
				{
					Output += TEXT(",") + Tag;
				}
			}
			// add the tags
			/* for (FName Tag : CachedVisibleActors[i]->Tags)
			{
				FString TagString = Tag.ToString();
				if (TagString != TEXT("Tagged"))
				{
					Output += TEXT(",") + TagString;
				}
			}*/

			// perform cross product and distance calculation

			if (APawn* OwnerChar = GetPawn())
			{
				FVector StarlightLocation = OwnerChar->GetActorLocation();
				FVector StarlightForward = OwnerChar->GetActorForwardVector();
				FVector StarlightToObj = CachedVisibleActors[i]->GetActorLocation() - StarlightLocation;

				// bearing
				FVector Cross = FVector::CrossProduct(StarlightForward, StarlightToObj);
				if (Cross.Z > 0.0f)
				{
					Output = Output + TEXT(",left");
				}
				else
				{
					Output = Output + TEXT(",right");
				}

				// distance
				Output = Output + TEXT(",") + FString::FromInt(StarlightToObj.Length());

				Output += TEXT(";");
			}
		}
	}

	for (int i = 0; i < ActorsWithTags.Num(); i++)
	{
		if (ActorsWithTags[i] != nullptr)
		{
			// add index
			Output = Output + ActorsWithTags[i]->GetName();

			if (UStarlightSeeComponent* See = ActorsWithTags[i]->GetComponentByClass<UStarlightSeeComponent>())
			{
				for (FString Tag : See->Tags)
				{
					Output += TEXT(",") + Tag;
				}
				Output += TEXT(";");
			}
		}
	}

	return Output;
}

void AStarlightGOAPController::SetPlayerFocusedComponent()
{
	if (PlayerFocusedStarlightSeeComponent)
	{
		PlayerFocusedStarlightSeeComponent->Tags.Remove(TEXT("PlayerFocused"));
		PlayerFocusedStarlightSeeComponent = nullptr;
	}
	if (ADuoQPlayerCharacter* PlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
	{
		FHitResult HitResult;
		FVector	   TargetLocation = PlayerCharacter->GetActorLocation() + PlayerCharacter->GetControlRotation().RotateVector(FVector(10000.0f, 0.0f, 0.0f));

		if (GetWorld()->LineTraceSingleByChannel(HitResult, PlayerCharacter->GetActorLocation(), TargetLocation, ECC_Visibility))
		{
			if (AActor* HitActor = HitResult.GetActor())
			{
				if (UStarlightSeeComponent* HitSeeComponent = HitActor->GetComponentByClass<UStarlightSeeComponent>())
				{
					HitSeeComponent->Tags.Add(TEXT("PlayerFocused"));
					PlayerFocusedStarlightSeeComponent = HitSeeComponent;
				}
			}
		}
	}
}

UGOAPAction* AStarlightGOAPController::AddKeyedActionToGOAPPlanner(FAtom Goal, const UClass* Class, FName ActionName, TArray<FAtom> Effects, TArray<FAtom> Preconditions, AActor* Target)
{
	if (UPriorityGOAPAction* Action = NewObject<UPriorityGOAPAction>(this, Class, ActionName))
	{
		// see if the action name is a grunt enemy prompt
		if (ActionName.ToString().Contains(FString(TEXT("SLPrompt"))))
		{
			float NewCost = FMath::RandRange(0.1, 0.15);
			// float NewCost = 0.0f;
			Action->cost = NewCost;
			Action->setCost(NewCost);
		}
		/*
		else
		{
			float NewCost = FMath::RandRange(0.3, 1.0);
			Action->cost = NewCost;
			Action->setCost(NewCost);
		}*/

		Action->setName(ActionName.ToString());
		Action->mWorld = Target->GetWorld();
		Action->setTarget(Target);
		Action->TargetActor = Target;

		Action->effects.Append(Effects);
		Action->preconditions.Append(Preconditions);
		Action->create_P_E();

		Action->KeyedGoals.Emplace(Goal);
		// Action->AddToRoot();

		planner->addKeyedAction(Goal.name, Action);
		SavingFromGarbo.Emplace(Action);
		return Action;
	}
	else
	{
		return nullptr;
	}
}

void AStarlightGOAPController::ResetDistanceTimer()
{
	bShouldWarnDistance = true;
}

void AStarlightGOAPController::debugInfo()
{
	FString name = GetPawn()->GetName();

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("\n"), false);
	if (!mStoredPlans.IsEmpty())
	{
		for (int i = 0; i < mStoredPlans[0].Num(); i++)
		{
			if (i != 0)
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("--->"));
			FString action = mStoredPlans[0][i]->GetName();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("" + action));
		}
	}
	if (controller)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("controller: " + this->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Plan generated by: " + name));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("\n"));
}
