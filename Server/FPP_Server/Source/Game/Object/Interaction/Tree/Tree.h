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
	void GenerateFruit();
	virtual void interact() override;
};

