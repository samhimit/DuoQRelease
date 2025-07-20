/**
	GOAP NPC: Goal-Oriented Action Planning for Non-Player Characters
	Copyright � 2022 Narratech Laboratories

	Authors: Diego Romero-Hombrebueno Santos, Mario S�nchez Blanco, Jos� Manuel Sierra Ramos, Daniel Gil Aguilar and Federico Peinado
	Website: https://narratech.com/project/goap-npc/
 */
#include "GOAPWorldState.h"

GOAPWorldState::GOAPWorldState() {}

GOAPWorldState::~GOAPWorldState() {}

GOAPWorldState::GOAPWorldState(const TMap<FString, bool>& a)
{
	atoms = a;
}

bool GOAPWorldState::operator==(GOAPWorldState  w)
{
	for(auto atom : w.getAtoms())
	{
		auto Found = atoms.Find(atom.Key);
		if(Found)
		{
			if(*Found != atom.Value)
				return false;
		}
		else
		{
			return false;
		}
	}
	return true;
	//return atoms.Num() == w.getAtoms().Num() && std::equal(atoms.begin(), atoms.end(), w.getAtoms().begin());
}

bool GOAPWorldState::isIncluded(GOAPWorldState w)
{
	for (auto requirement : w.getAtoms())
	{
		auto it = atoms.Find(requirement.Key);
		if (it)
		{
			if (*it != requirement.Value)
				return false;
		}
		else return false;
	}
	return true;

}

TMap<FString, bool>& GOAPWorldState::getAtoms()
{
	return atoms;
}

void GOAPWorldState::setAtoms(const TMap<FString, bool>& a)
{
	atoms = a;
}

void GOAPWorldState::addAtom(FString name, bool value)
{
	atoms.FindOrAdd(name) = value;
}

void GOAPWorldState::removeAtom(FString name)
{
	atoms.Remove(name);
}

void GOAPWorldState::cleanAtoms()
{
	atoms.Empty();
}

//find atoms that exist in the current world space and set the bool value
//to those in the second world space
void GOAPWorldState::joinWorldState(GOAPWorldState w)
{
	for (auto atom : w.getAtoms())
	{
		atoms.FindOrAdd(atom.Key) = atom.Value;
	}
}

bool GOAPWorldState::isEmpty()
{
	return atoms.Num() == 0;
}
