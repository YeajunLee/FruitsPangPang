#include "Object.h"


Object::Object()
	:x(0), y(0), z(0)
	, rx(0), ry(0), rz(0), rw(1)
	, sx(1), sy(1), sz(1)
	,_id(-1)
	,_type(OBJTYPE::NONE)
{

}

Object::~Object()
{

}