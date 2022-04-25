#pragma once
#include "../../../../../Protocol/protocol.h"
class Object
{
public:
	Object();
	virtual ~Object();
	enum class OBJTYPE{NONE,PLAYER,NPC};
	float	x, y, z;
	float	rx, ry, rz, rw;
	float	sx, sy, sz;
	int		_id;
	OBJTYPE	_otype;


	bool isPlayer() { return _id < MAX_USER; }
};

