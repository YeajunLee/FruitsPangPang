#pragma once

#include "../Object.h"


class Interaction : public Object
{
public:
	enum class INTERTYPE { NONE, TREE, PUNNET };	//interact_type [tree,punnet]

	Interaction(OBJTYPE type = OBJTYPE::INTERACT, INTERTYPE itype = INTERTYPE::NONE);
	virtual ~Interaction();
	virtual void interact();
	virtual void GenerateFruit();
	INTERTYPE _itype;
};

