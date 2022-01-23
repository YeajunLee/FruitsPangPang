#pragma once
#include "../Interact.h"
#include <atomic>
class Tree : public Interact
{
public:
	Tree();
	virtual ~Tree();

	std::atomic<bool> canHarvest;
};

