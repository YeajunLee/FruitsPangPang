#include <iostream>
#include <random>
#include "Character.h"
#include "../../Network/Network.h"


using namespace std;

Character::Character(OBJTYPE type, STATE state)
	:_state(state)
	, _is_active(true)
	,maxhp(PLAYER_HP)
	,hp(maxhp)
	, mKillCount(0)
	,mDeathCount(0)
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
		if(index != 2)
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
	mDeathCount++;
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

void Character::HurtBy(const int& damageCauserType, const int& attacker)
{
	switch (damageCauserType)
	{

	case static_cast<int>(FRUITTYPE::T_TOMATO) : {
		Hurt(3, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_QUIUI) : {
		Hurt(10, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_APPLE) : {
		Hurt(10, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_WATERMELON) : {
		Hurt(10, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_PINEAPPLE) : {
		Hurt(10, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_PUMPKIN) : {
		Hurt(10, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_GREENONION) : {
		Hurt(10, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_CARROT) : {
		Hurt(10, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_NUT) : {
		Hurt(7, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_BANANA) : {
		Hurt(10, attacker);
		break;
	}
	default: {
		cout << "HurtBy : Unknown Damage Type\n";
		break;
	}

	}
}

void Character::Hurt(const int& damage, const int& attacker)
{
	if (hp <= 0) return;	//0일때 다치면 안됨.
	hp = max(hp - damage, 0);
	send_update_userstatus_packet(_id);
	cout << _id << "의 이후 hp : " << hp << endl;
	if (hp <= 0)
	{
		if (0 <= attacker && attacker <= MAX_USER)
		{
			Character* attackerCharacter = reinterpret_cast<Character*>(objects[attacker]);
			attackerCharacter->mKillCount++;
		}
		Die();
	}
}


void Character::Heal(const int& amount)
{
	hp = min(hp + amount, maxhp);
	send_update_userstatus_packet(_id);
	cout << _id << "의 이후 hp : " << hp << endl;
}


void Character::Respawn(const int& RespawnSpot)
{
	hp = maxhp;
	rx = 0, ry = 0, rz = 0, rw = 1;
	switch (RespawnSpot)
	{
	case 0:
		x = 18090, y = 17480, z = 100;
		break;
	case 1:
		x = 9880, y = 18390, z = 100;
		break;
	case 2:
		x = 2200, y = 18140, z = 100;
		break;
	case 3:
		x = 18350, y = 9610, z = 100;
		break;
	case 4:
		x = 1750, y = 10220, z = 100;
		break;
	case 5:
		x = 17980, y = 1780, z = 100;
		break;
	case 6:
		x = 9730, y = 1620, z = 100;
		break;
	case 7:
		x = 1990, y = 1740, z = 100;
		break;
	case 8:	{
		random_device rd;
		mt19937 rng(rd());
		uniform_int_distribution<int> randomRespawn(0, 7);
		Respawn(randomRespawn(rng));
		break; 
		}
	}
}