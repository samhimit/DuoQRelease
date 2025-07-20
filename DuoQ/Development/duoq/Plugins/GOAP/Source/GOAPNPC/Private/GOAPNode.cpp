/**
	GOAP NPC: Goal-Oriented Action Planning for Non-Player Characters
	Copyright � 2022 Narratech Laboratories

	Authors: Diego Romero-Hombrebueno Santos, Mario S�nchez Blanco, Jos� Manuel Sierra Ramos, Daniel Gil Aguilar and Federico Peinado
	Website: https://narratech.com/project/goap-npc/
 */
#include "GOAPNode.h"

GOAPNode::GOAPNode()
{
	action = NULL;
	g = 0;
	h = 0;
}

GOAPNode::GOAPNode(UGOAPAction* a)
{
	action = a;
	if (a != NULL)g = a->getCost();
	else g = 0;
	h = 0;
}

bool GOAPNode::operator==(GOAPNode n) const
{
	return action == n.getAction();
}


GOAPWorldState GOAPNode::getWorld() const
{
	return world;
}

int GOAPNode::getH() const
{
	return h;
}

float GOAPNode::getG() const
{
	return g;
}

float GOAPNode::getF() const
{
	return g + h;
}

int GOAPNode::getParent() const
{
	return parent;
}

UGOAPAction* GOAPNode::getAction() const
{
	return action;
}

void GOAPNode::setWorld(GOAPWorldState w)
{
	this->world = w;
}

void GOAPNode::setH(int value)
{
	this->h = value;
}

void GOAPNode::setH(GOAPWorldState w)
{
	for (auto it : world.getAtoms())
	{
		auto aux = w.getAtoms().Find(it.Key);
		if (aux)
		{
			if (it.Value != *aux) ++h;
		}
		else ++h;
	}
}

void GOAPNode::setG(GOAPNode p)
{
	this->g += p.getG();
}

void GOAPNode::setParent(int p)
{
	this->parent = p;
}

void GOAPNode::setAction(UGOAPAction* a)
{
	this->h = 0;
	this->g = a->getCost();
	this->action = a;
}
