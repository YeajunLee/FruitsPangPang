#include "Heal.h"
#include "../../../Network/Network.h"
#include "../../Character/Character.h"

#include <random>
#include <iostream>

using namespace std;



Heal::Heal()
	:Interaction(Object::OBJTYPE::INTERACT, Interaction::INTERTYPE::HEAL)
	, _ftype(FRUITTYPE::T_HEAL)
{
}

Heal::~Heal()
{

}


void Heal::interact(Object* interactobj)
{
	Interaction::interact(interactobj);

	Character* character = reinterpret_cast<Character*>(interactobj);

	CanInteract_lock.lock();
	if (false == this->CanInteract)
	{
		CanInteract_lock.unlock();
		cout << "interact fail !" << endl;
		return;
	}
	else {
		this->CanInteract = false;
		CanInteract_lock.unlock();
	}

	character->Heal(10);

	for (auto& other : objects)
	{
		if (!other->isPlayer()) break;
		auto player = reinterpret_cast<Character*>(other);
		if (player->_state == Character::STATE::ST_INGAME)
		{
			//cout << "���Ϲڽ� �Ծ��ٰ� �����ϴ�" <<packet->obj_id<<"," << endl;
			send_update_interstat_packet(other->_id, this->_id - HEALID_START, false, INTERACT_TYPE_HEAL);
		}
	}

	cout << "Heal Interact" << endl;
	Timer_Event instq;
	instq.exec_time = chrono::system_clock::now() + 5000ms;
	instq.type = Timer_Event::TIMER_TYPE::TYPE_HEAL_RESPAWN;
	instq.object_id = _id;
	timer_queue.push(instq);
}

void Heal::ResetObject()
{
	Interaction::ResetObject();
}

void Heal::GenerateFruit()
{
	Interaction::GenerateFruit();


	CanInteract_lock.lock();
	this->CanInteract = true;
	CanInteract_lock.unlock();


	for (auto& other : objects)
	{
		if (!other->isPlayer()) break;
		auto player = reinterpret_cast<Character*>(other);
		//��Ƽ������� ���ؼ� �Ʒ� ���ǹ��� �߸� ����ȴ� �ϴ��� ������ send_packet���� �ɷ����� ���� ���輺�� ����.
		if (Character::STATE::ST_INGAME == player->_state)
		{
			//cout << "���� �����ƴٰ� �����ϴ�" << endl;
			send_update_interstat_packet(other->_id, this->_id - HEALID_START, true, INTERACT_TYPE_HEAL, static_cast<int>(this->_ftype));
		}
	}
}