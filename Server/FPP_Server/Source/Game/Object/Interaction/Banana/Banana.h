#pragma once
#include "../Interaction.h"

class Banana : public Interaction
{
public:
	enum class STATE { ST_FREE,ST_JUDGE, ST_ACTIVE};	//FREE = �ٳ��� ��ȯX, JUDGE = �ٳ��� �浹 ������, ACTIVE = �ٳ��� ��ȯ O
	Banana();
	virtual ~Banana();
	FRUITTYPE _ftype;

	virtual void ResetObject() override;
	virtual void GenerateFruit() override;
	virtual void interact(class Object* interactobj) override;
	std::mutex state_lock;
	Banana::STATE _state;
};

