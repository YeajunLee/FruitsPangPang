#pragma once
#include "../Interaction.h"
#include <atomic>

class Punnet : public Interaction
{
public:
	Punnet();
	virtual ~Punnet();
	FRUITTYPE _ftype;

	virtual void GenerateFruit() override;
	virtual void interact(class Object* interactobj) override;
};

