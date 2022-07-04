#pragma once
#include "../Object.h"
class Character : public Object
{
public:
	enum class CTYPE {NONE,PLAYER,NPC};
	Character();
	virtual ~Character();

	CTYPE _ctype;
};

