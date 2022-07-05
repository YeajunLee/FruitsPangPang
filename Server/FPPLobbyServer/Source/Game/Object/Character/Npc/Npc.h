#pragma once
#include "../Character.h"
#include <array>
class Npc : public Character
{
public:
	Npc(OBJTYPE type = OBJTYPE::NPC);
	virtual ~Npc();
	int NumberofItemsInStore;
	std::array<std::pair<unsigned char, short>, 255> Shop;
};

