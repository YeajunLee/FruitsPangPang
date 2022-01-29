#include "Tree.h"


Tree::Tree()
	:Interact(Object::OBJTYPE::INTERACT,Interact::INTERTYPE::TREE)
	,canHarvest(true)
	,T_TYPE(FRUITTYPE::T_TOMATO)
{

}

Tree::~Tree()
{

}
