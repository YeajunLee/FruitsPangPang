#pragma once
#include "../Interaction.h"
#include <atomic>

enum class TREETYPE
{
	NONE,GREEN,ORANGE
};


class Tree : public Interaction
{
public:
	Tree();
	Tree(TREETYPE ttype);
	virtual ~Tree();

	std::atomic<bool> canHarvest;
	FRUITTYPE _ftype;
	TREETYPE _ttype;
	virtual void GenerateFruit() override;
	virtual void interact() override;
};

