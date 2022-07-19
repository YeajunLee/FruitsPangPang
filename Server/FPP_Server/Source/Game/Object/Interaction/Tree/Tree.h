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

	FRUITTYPE _ftype;
	TREETYPE _ttype;
	virtual void ResetObject() override;
	virtual void GenerateFruit() override;
	virtual void interact(class Object* interactobj) override;
};

