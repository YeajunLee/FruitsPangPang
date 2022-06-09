#include "Tree.h"
#include <random>
#include <iostream>
#include "../../../Network/Network.h"
#include "../../Character/Character.h"
using namespace std;

Tree::Tree()
	:Interaction(Object::OBJTYPE::INTERACT, Interaction::INTERTYPE::TREE)
	,_ftype(FRUITTYPE::NONE)
	,_ttype(TREETYPE::NONE)
{

}


Tree::Tree(TREETYPE ttype)
	:Interaction(Object::OBJTYPE::INTERACT, Interaction::INTERTYPE::TREE)
	, _ttype(ttype)
	, _ftype(FRUITTYPE::NONE)
{

	/*나중에 랜덤으로 바꿔야함*/
	if (ttype == TREETYPE::GREEN)
	{
		_ftype = FRUITTYPE::T_TOMATO;
	}
	else if (ttype == TREETYPE::ORANGE)
	{
		_ftype = FRUITTYPE::T_WATERMELON;
	}
}


Tree::~Tree()
{

}

void Tree::interact(Object* interactobj)
{
	Interaction::interact(interactobj);
	Character* character = reinterpret_cast<Character*>(interactobj);

	CanInteract_lock.lock();
	if (false == this->CanInteract)
	{
		CanInteract_lock.unlock();
		cout << " 수확할 수 없습니다!" << endl;
		return;
	}
	else {
		this->CanInteract = false;
		CanInteract_lock.unlock();
	}

	FPP_LOG("플레이어[%d]가 [%d]번째 나무에서 [%d]타입의 아이템 획득", character->_id, this->_id, this->_ftype);
	switch (this->_ttype)
	{
	case TREETYPE::GREEN:
		character->UpdateInventorySlotAtIndex(0, this->_ftype, 10);
		send_update_inventory_packet(character->_id, 0);
		break;
	case TREETYPE::ORANGE:
		character->UpdateInventorySlotAtIndex(1, this->_ftype, 5);
		send_update_inventory_packet(character->_id, 1);
		break;
	}

	for (auto& other : objects)
	{
		if (!other->isPlayer()) break;
		auto player = reinterpret_cast<Character*>(other);
		if (player->_state == Character::STATE::ST_INGAME)
		{
			//cout << "과일나무 떨어졌다고 보냅니다"<<packet->obj_id<<"," << endl;
			//TREEID_START를 빼주는 것은 서버의 tree id는 시작이 TREEID_START 부터이지만 클라는 0번째부터 시작이기 때문에 조정이 필요함.
			send_update_interstat_packet(other->_id, this->_id - TREEID_START, false, INTERACT_TYPE_TREE);
		}
	}

	Timer_Event instq;
	instq.exec_time = chrono::system_clock::now() + 5000ms;
	instq.type = Timer_Event::TIMER_TYPE::TYPE_TREE_RESPAWN;
	instq.object_id = _id;
	timer_queue.push(instq);
}

void Tree::GenerateFruit()
{
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> GreenFruit(static_cast<int>(FRUITTYPE::T_TOMATO), static_cast<int>(FRUITTYPE::T_APPLE));
	uniform_int_distribution<int> OrangeFruit(static_cast<int>(FRUITTYPE::T_WATERMELON), static_cast<int>(FRUITTYPE::T_PUMPKIN));

	if (_ttype == TREETYPE::GREEN)
	{
		_ftype = static_cast<FRUITTYPE>(GreenFruit(rng));
	}
	else if (_ttype == TREETYPE::ORANGE)
	{
		_ftype = static_cast<FRUITTYPE>(OrangeFruit(rng));
	}
	else {
		cout << "[BUG] Unvalid Tree Type! : " << static_cast<int>(_ttype) << endl;
	}



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
			//cout << "과일나무 생성됐다고 보냅니다" << endl;
			send_update_interstat_packet(other->_id, this->_id - TREEID_START, true, INTERACT_TYPE_TREE, static_cast<int>(this->_ftype));
		}
	}
}