#include "Punnet.h"
#include "../../../Network/Network.h"

#include "../../Character/Character.h"
#include <random>
#include <iostream>

using namespace std;



Punnet::Punnet()
	:Interaction(Object::OBJTYPE::INTERACT, Interaction::INTERTYPE::PUNNET)
	,_ftype(FRUITTYPE::T_BANANA)
{

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> Punnet(static_cast<int>(FRUITTYPE::T_GREENONION), static_cast<int>(FRUITTYPE::T_BANANA));

	_ftype = static_cast<FRUITTYPE>(Punnet(rng));
}

Punnet::~Punnet()
{

}


void Punnet::interact(Object* interactobj)
{
	Interaction::interact(interactobj);


	Character* character = reinterpret_cast<Character*>(interactobj);


	CanInteract_lock.lock();
	if (false == this->CanInteract)
	{
		CanInteract_lock.unlock();
		//cout << "수확할 수 없습니다!" << endl;
		return;
	}
	else {
		this->CanInteract = false;
		CanInteract_lock.unlock();
	}

	//cout << "과일 받았습니다(과일상자)" << endl;
	if (this->_ftype == FRUITTYPE::T_GREENONION)
	{
		character->UpdateInventorySlotAtIndex(2, this->_ftype, 1);
	}
	else if (this->_ftype == FRUITTYPE::T_CARROT)
	{
		character->UpdateInventorySlotAtIndex(2, this->_ftype, 1);
	}
	else if (this->_ftype == FRUITTYPE::T_BANANA)
	{
		character->UpdateInventorySlotAtIndex(4, this->_ftype, 3);
	}
	else
	{
		character->UpdateInventorySlotAtIndex(3, this->_ftype, 5);
	}

	for (auto& other : objects)
	{
		if (!other->isPlayer()) break;
		auto player = reinterpret_cast<Character*>(other);
		if (player->_state == Character::STATE::ST_INGAME)
		{
			//cout << "과일박스 먹었다고 보냅니다" <<packet->obj_id<<"," << endl;
			send_update_interstat_packet(other->_id, this->_id - PUNNETID_START, false, INTERACT_TYPE_PUNNET);
		}
	}

	cout << "Punnet Interact" << endl;
	Timer_Event instq;
	instq.exec_time = chrono::system_clock::now() + 5000ms;
	instq.type = Timer_Event::TIMER_TYPE::TYPE_PUNNET_RESPAWN;
	instq.object_id = this->_id;
	timer_queue.push(instq);
}

void Punnet::ResetObject()
{
	Interaction::ResetObject();

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> Punnet(static_cast<int>(FRUITTYPE::T_GREENONION), static_cast<int>(FRUITTYPE::T_BANANA));

	_ftype = static_cast<FRUITTYPE>(Punnet(rng));
}

void Punnet::GenerateFruit()
{
	Interaction::GenerateFruit();

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> Punnet(static_cast<int>(FRUITTYPE::T_GREENONION), static_cast<int>(FRUITTYPE::T_BANANA));

	_ftype = static_cast<FRUITTYPE>(Punnet(rng));
	cout << "_ftype : " << (int)_ftype << endl;


	CanInteract_lock.lock();
	this->CanInteract = true;
	CanInteract_lock.unlock();


	for (auto& other : objects)
	{
		if (!other->isPlayer()) break;
		auto player = reinterpret_cast<Character*>(other);
		//멀티스레드로 인해서 아랫 조건문이 잘못 실행된다 하더라도 어차피 send_packet에서 걸러져서 딱히 위험성이 없다.
		if (player->_state == Character::STATE::ST_INGAME)
		{
			//cout << "과일바구니 생성됐다고 보냅니다" << endl;
			send_update_interstat_packet(other->_id, this->_id - PUNNETID_START, true, INTERACT_TYPE_PUNNET, static_cast<int>(this->_ftype));
		}
	}
}