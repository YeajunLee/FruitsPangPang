#pragma once
#include "../Object.h"

class Interact : public Object
{
public:
	enum class INTERTYPE { NONE, TREE, HEAL };	//interact_type [tree,heal]

	Interact(OBJTYPE type = OBJTYPE::INTERACT,INTERTYPE itype = INTERTYPE::NONE);
	virtual ~Interact();
	INTERTYPE _itype;
};

