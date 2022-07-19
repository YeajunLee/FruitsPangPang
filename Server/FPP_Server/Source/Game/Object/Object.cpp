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

void Object::ResetObject()
{
	x = 0.f, y = 0.f, z = 0.f;
	rx = 0.f, ry = 0.f, rz = 0.f, rw = 0.f;
	sx = 0.f, sy = 0.f, sz = 0.f;
}
