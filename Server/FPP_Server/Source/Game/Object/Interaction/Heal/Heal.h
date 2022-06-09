#pragma once
#include "../Interaction.h"
class Heal : public Interaction
{
public:
	Heal();
	virtual ~Heal();
	FRUITTYPE _ftype;

	virtual void GenerateFruit() override;
	virtual void interact(class Object* interactobj) override;
};

