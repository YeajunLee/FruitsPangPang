#pragma once
#include "../Interaction.h"
#include <atomic>

class Punnet : public Interaction
{
public:
	Punnet();
	virtual ~Punnet();
	std::atomic<bool> canHarvest;
	FRUITTYPE _ftype;

	virtual void GenerateFruit() override;
	virtual void interact() override;
};

