#include "Punnet.h"
#include "../../../Network/Network.h"

#include <random>
#include <iostream>

using namespace std;



Punnet::Punnet()
	:Interaction(Object::OBJTYPE::INTERACT, Interaction::INTERTYPE::PUNNET)
	, canHarvest(true)
	,_ftype(FRUITTYPE::T_DURIAN)
{

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> Punnet(static_cast<int>(FRUITTYPE::T_GREENONION), static_cast<int>(FRUITTYPE::T_HEAL));

	while (1)
	{
		_ftype = static_cast<FRUITTYPE>(Punnet(rng));
		if ((FRUITTYPE::T_BANANA != _ftype))
			break;
	}
}

Punnet::~Punnet()
{

}


void Punnet::interact()
{
	Interaction::interact();

	cout << "Punnet Interact" << endl;
	Timer_Event instq;
	instq.exec_time = chrono::system_clock::now() + 5000ms;
	instq.type = Timer_Event::TIMER_TYPE::TYPE_PUNNET_RESPAWN;
	instq.object_id = _id;
	timer_queue.push(instq);
}

void Punnet::GenerateFruit()
{
	Interaction::GenerateFruit();

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> Punnet(static_cast<int>(FRUITTYPE::T_GREENONION), static_cast<int>(FRUITTYPE::T_HEAL));

	while (1)
	{
		_ftype = static_cast<FRUITTYPE>(Punnet(rng));
		if ((FRUITTYPE::T_BANANA != _ftype))
			break;
	}
	cout << "_ftype : " << (int)_ftype << endl;
}