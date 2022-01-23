#pragma once
#include "../../../../../Protocol/protocol.h"

class Object
{
public:
	Object();
	virtual ~Object();
	enum class OBJTYPE { NONE, PLAYER, INTERACT };
	float x, y, z;
	float rx, ry, rz, rw;
	float sx, sy, sz;
	int		_id;
	OBJTYPE	_type;

	bool isPlayer() { return _id < MAX_USER; }
	bool isInteract() { return _id > 8; }
};
