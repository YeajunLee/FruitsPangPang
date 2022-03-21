#pragma once
#include "../../../../../Protocol/protocol.h"


enum class FRUITTYPE { NONE, T_TOMATO,T_QUIUI,T_APPLE,T_WATERMELON,T_PINEAPPLE,T_PUMPKIN,T_GREENONION,T_CARROT,T_DURIAN,T_NUT,T_BANANA,T_HEAL };

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

