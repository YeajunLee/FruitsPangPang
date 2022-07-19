#include "Interaction.h"



Interaction::Interaction(OBJTYPE type, INTERTYPE itype)
	:_itype(itype)
	,CanInteract(true)
{
	_type = type;
}

Interaction::~Interaction()
{

}

void Interaction::ResetObject()
{
	Object::ResetObject();
	CanInteract = true;
}


void Interaction::interact(Object* interactobj)
{

}

void Interaction::GenerateFruit()
{

}