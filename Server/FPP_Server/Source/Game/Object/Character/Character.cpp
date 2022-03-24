#include <iostream>
#include "Character.h"
#include "../../Network/Network.h"


using namespace std;

Character::Character(OBJTYPE type, STATE state)
	:_state(state)
	, _is_active(true)
	,maxhp(PLAYER_HP)
	,hp(maxhp)
{
	_type = type;
}

Character::~Character()
{

}



void Character::recvPacket()
{
	ZeroMemory(&wsa_ex_recv.getWsaOver(), sizeof(wsa_ex_recv.getWsaOver()));
	wsa_ex_recv.getWsaBuf().buf = reinterpret_cast<char*>(wsa_ex_recv.getBuf() + _prev_size);
	wsa_ex_recv.getWsaBuf().len = BUFSIZE - _prev_size;

	DWORD flags = 0;
	int ret = WSARecv(_socket, &wsa_ex_recv.getWsaBuf(), 1, 0, &flags, &wsa_ex_recv.getWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			error_display(err);
		}
	}
}

void Character::sendPacket(void* packet, int bytes)
{
	WSA_OVER_EX* wsa_ex = new WSA_OVER_EX(CMD_SEND, bytes, packet);
	int ret = WSASend(_socket, &wsa_ex->getWsaBuf(), 1, 0, 0, &wsa_ex->getWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			error_display(err);
		}
	}
}

void Character::UpdateInventorySlotAtIndex(const int& index, FRUITTYPE itemcode, const int& amount)
{
	auto& slot = mSlot[index];
	if (slot.type == itemcode)
	{
		slot.amount += amount;
	}
	else {
		slot.type = itemcode;
		slot.amount = amount;
	}
}

void Character::Die()
{
	Timer_Event instq;
	instq.player_id = _id;
	instq.type = Timer_Event::TIMER_TYPE::TYPE_PLAYER_RESPAWN;
	instq.exec_time = chrono::system_clock::now() + 5000ms;
	timer_queue.push(instq);
	cout << "플레이어 " << _id << "사망\n";
	for (auto& other : objects) {
		if (!other->isPlayer()) break;
		auto character = reinterpret_cast<Character*>(other);

		character->state_lock.lock();
		if (Character::STATE::ST_INGAME == character->_state)
		{
			character->state_lock.unlock();
			send_die_packet(character->_id, _id);
		}
		else character->state_lock.unlock();
	}


}

void Character::HurtBy(const int& damageCauserType)
{
	switch (damageCauserType)
	{

	case static_cast<int>(FRUITTYPE::T_TOMATO) : {
		Hurt(3);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_QUIUI) : {
		Hurt(10);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_APPLE) : {
		Hurt(10);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_WATERMELON) : {
		Hurt(10);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_PINEAPPLE) : {
		Hurt(10);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_PUMPKIN) : {
		Hurt(10);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_GREENONION) : {
		Hurt(10);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_CARROT) : {
		Hurt(10);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_NUT) : {
		Hurt(7);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_BANANA) : {
		Hurt(10);
		break;
	}
	default: {
		cout << "HurtBy : Unknown Damage Type\n";
		break;
	}

	}
}

void Character::Hurt(const int& damage)
{
	if (hp <= 0) return;	//0일때 다치면 안됨.
	hp = max(hp - damage, 0);
	send_update_userstatus_packet(_id);
	cout << _id << "의 이후 hp : " << hp << endl;
	if (hp <= 0)
	{
		Die();
	}
}


void Character::Heal(const int& amount)
{
	hp = min(hp + amount, maxhp);
	send_update_userstatus_packet(_id);
	cout << _id << "의 이후 hp : " << hp << endl;
}