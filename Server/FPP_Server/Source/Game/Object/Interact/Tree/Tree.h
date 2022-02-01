#pragma once
#include "../Interact.h"
#include <atomic>

enum class TREETYPE
{
	NONE,GREEN,ORANGE
};


class Tree : public Interact
{
public:
	Tree();
	Tree(TREETYPE ttype);
	virtual ~Tree();

	std::atomic<bool> canHarvest;
	FRUITTYPE _ftype;
	TREETYPE _ttype;
	void GenerateFruit();
};

