/**
	GOAP NPC: Goal-Oriented Action Planning for Non-Player Characters
	Copyright � 2022 Narratech Laboratories

	Authors: Diego Romero-Hombrebueno Santos, Mario S�nchez Blanco, Jos� Manuel Sierra Ramos, Daniel Gil Aguilar and Federico Peinado
	Website: https://narratech.com/project/goap-npc/
 */
#include "GOAPPlanner.h"

GOAPPlanner::GOAPPlanner() {}

GOAPPlanner::~GOAPPlanner() {}

GOAPPlanner::GOAPPlanner(GOAPWorldState* c, GOAPWorldState* g, const TArray<UGOAPAction*>& a)
{
	currentWorld = c;
	goal = g;
	actions = a;
}

GOAPNode GOAPPlanner::lowestFinList(const TArray<GOAPNode>& opList)
{
	GOAPNode node;

	float minF = MAX_FLT;
	for (const GOAPNode& n : opList)
	{
		if ((n.getF()) < minF)
		{
			node = n;
			minF = n.getF();
		}
	}

	return node;
}

bool containsNode(GOAPNode node, const TArray<GOAPNode>& list)
{
	bool contains = false;
	for (const GOAPNode& n : list)
	{
		if (n == node)
		{
			contains = true;
			break;
		}
	}
	return contains;
}

TArray<GOAPNode> GOAPPlanner::getAdjacent(GOAPNode current, const TArray<UGOAPAction*>& vActions, APawn* p)
{
	TArray<GOAPNode> adjacentNodes;
	GOAPNode adjacent;
	GOAPWorldState world;

	for (int i = 0; i < vActions.Num(); ++i)
	{
		if(vActions[i]->IsValidLowLevel())
		{
			// Checks if the action can be performed from the current world.
			const bool bPredoncitionsAreMet = current.getWorld().isIncluded(vActions[i]->getPreconditions());
			// Checks if the action is the same as the current one. (This can be deleted if you want your AI to perform the same action consecutively).
			const bool bSameActionAsBefore = current.getAction() == vActions[i];
			// Checks the procedural precondition of the action.
			const bool bProceduralPreconditionFulfilled = vActions[i]->checkProceduralPrecondition(p);
			getGoal();

			if (bPredoncitionsAreMet && !bSameActionAsBefore && bProceduralPreconditionFulfilled)
			{
				world = current.getWorld(); // Saves the current world.
				world.joinWorldState(vActions[i]->getEffects()); // Applies effects of the action to the saved world.
				adjacent.setWorld(world); // Sets the adjacet node's world.
				adjacent.setAction(vActions[i]); // Sets the adjacet node's action. 
				adjacentNodes.Push(adjacent); // Includes the adjacent node in the list.
			}
		}
	}
	return adjacentNodes;
}

TArray<UGOAPAction*> GOAPPlanner::generatePlan(APawn* p)
{
	//KeyedActionLock.Lock();
	TArray<UGOAPAction*> sol;

	GOAPNode start; start.setWorld(*currentWorld); start.setParent(-1);
	GOAPNode last;
	openList.Empty();
	closedList.Empty();
	openList.Push(start);
	bool continues = true;
	bool goalReached = false;
	
	TArray<UGOAPAction*> AvailableActions = TArray<UGOAPAction*>(actions);
	if(goal->getAtoms().Num() > 0)
	{
		// this only works because we have one goal in every world state :,))
		FString Goal = goal->getAtoms().begin()->Key;
		if(TArray<UGOAPAction*>* KeyedActionsForGoal = keyedActions.Find(Goal))
		{
			AvailableActions.Append(KeyedActionsForGoal->GetData(),KeyedActionsForGoal->Num());
		}
	}

	// Search and create the cheapest path between actions having into account their preconditions, effects and cost.
	while (continues)
	{
		GOAPNode current = lowestFinList(openList);
		openList.Remove(current);
		closedList.Push(current);
		int pos = closedList.Num() - 1;

		// When the current plan reaches the goal, the plan stops.
		if (current.getWorld().isIncluded(*goal))
		{
			last = current;
			continues = false;
			goalReached = true;
			break;
		}
		// Get adjacents of actual node.
		TArray<GOAPNode> adjacents = getAdjacent(current, AvailableActions, p);

		// Explore adjacent nodes.
		for (GOAPNode& adjacent : adjacents)
		{
			// If the adjacent node isn't in the open list, it is added.
			if (!containsNode(adjacent, openList))
			{
				adjacent.setParent(pos);
				adjacent.setH(current.getWorld());
				adjacent.setG(current);
				openList.Push(adjacent);
			}
			// If current path to adjacent node is cheaper than the previous one, the path changes. 
			else if (adjacent.getG() > adjacent.getG() + current.getG())
			{
				adjacent.setParent(pos);
				adjacent.setG(current);
			}
		}

		// If open list is empty or the algorithm reach the maximum depth, the plan stops.
		if (openList.Num() == 0 || closedList.Num() > getMaxDepth())
		{
			continues = false;
		}
	}

	// Extracts the plan's path in reverse from closed list and copy it to a new variable.
	if (goalReached)
	{
		GOAPNode planNode = last;
		while (!(planNode == start))
		{
			sol.Push(planNode.getAction());
			planNode = closedList[planNode.getParent()];
		}
	}
	//KeyedActionLock.Unlock();
	return sol;
}

void GOAPPlanner::addAction(UGOAPAction* a)
{
	KeyedActionLock.Lock();
	this->actions.Push(a);
	KeyedActionLock.Unlock();
}

void GOAPPlanner::addKeyedAction(FString goalName, TArray<UGOAPAction*> actionArray)
{
	KeyedActionLock.Lock();
	this->keyedActions.FindOrAdd(goalName).Append(actionArray);
	KeyedActionLock.Unlock();
}

void GOAPPlanner::addKeyedAction(FString goalName, UGOAPAction* Action)
{
	KeyedActionLock.Lock();
	this->keyedActions.FindOrAdd(goalName).Emplace(Action);
	KeyedActionLock.Unlock();
}

void GOAPPlanner::removedKeyedActions(FString goalName)
{
	KeyedActionLock.Lock();
	this->keyedActions.Remove(goalName);
	KeyedActionLock.Unlock();
}

void GOAPPlanner::removeAction(UGOAPAction* a)
{
	KeyedActionLock.Lock();
	this->actions.Remove(a);
	KeyedActionLock.Unlock();
}

TArray<UGOAPAction*> GOAPPlanner::getKeyedActions(FString goalName)
{
	return this->keyedActions.FindOrAdd(goalName);
}

GOAPWorldState GOAPPlanner::getGoal()
{
	return *goal;
}
void GOAPPlanner::setGoal(GOAPWorldState* g)
{
	this->goal = g;
}
GOAPWorldState GOAPPlanner::getCurrentWorld()
{
	return *currentWorld;
}
void GOAPPlanner::setCurrentWorld(GOAPWorldState* w)
{
	this->currentWorld = w;
}

int GOAPPlanner::getMaxDepth() {
	return maxDepth;
}

void GOAPPlanner::setMaxDepth(int md) {
	maxDepth = md;
}
