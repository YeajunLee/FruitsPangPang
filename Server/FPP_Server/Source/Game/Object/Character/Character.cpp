#include <iostream>
#include <random>
#include "Character.h"
#include "../../Network/Network.h"


using namespace std;

Character::Character(OBJTYPE type, STATE state)
	:_state(state)
	,maxhp(PLAYER_HP)
	,hp(maxhp)
	, mKillCount(0)
	,mDeathCount(0)
	,bAi(false)
	,skintype(0)
	,_prev_size(0)
	,_socket(INVALID_SOCKET)
{
	_type = type;
}

Character::~Character()
{

}


void Character::PreRecvPacket(unsigned char* RemainMsg,int RemainBytes)
{

	//player->_prev_size = 0;
	//ZeroMemory(wsa_ex->getBuf(), sizeof(wsa_ex->getBuf()));
	_prev_size = 0;
	if (RemainBytes > 0)
	{
		_prev_size = RemainBytes;
		memcpy(wsa_ex_recv.getBuf(), RemainMsg, RemainBytes);
	}
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
			Disconnect(_id);
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
			Disconnect(_id);
		}
	}
}

void Character::ResetObject()
{
	Object::ResetObject();
	_state = STATE::ST_FREE;
	maxhp = PLAYER_HP;
	hp = maxhp;
	mKillCount = 0;
	mDeathCount = 0;
	bAi = false;
	skintype = 0;
	_prev_size = 0;
	closesocket(_socket);
	_id = -1;
	_socket = INVALID_SOCKET;
	for (auto& slot : mSlot)
		slot.ResetInventory();
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

	send_update_inventory_packet(_id, index);
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
	case static_cast<int>(FRUITTYPE::T_KIWI) : {
		Hurt(3, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_APPLE) : {
		Hurt(4, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_WATERMELON) : {
		Hurt(10, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_PINEAPPLE) : {
		Hurt(5, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_PUMPKIN) : {
		Hurt(8, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_GREENONION) : {
		cout << " Hurt By GreeenOnion\n";
		Hurt(7, attacker);
		break;
	}
	case static_cast<int>(FRUITTYPE::T_CARROT) : {
		cout << " Hurt By Carrot\n";
		Hurt(7, attacker);
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
	if (0 > attacker || attacker >= MAX_USER) return;	//이상한얘가 때리는거에 맞으면 안됨.
	hp = max(hp - damage, 0);
	send_update_userstatus_packet(_id);
	FPP_LOG("User[%d] Hit [%d] 남은 hp : %d ", attacker, _id, hp.load());
	//cout << _id << "의 이후 hp : " << hp << endl;
	if (hp <= 0)
	{
		short userDeathcount[8];
		short userKillcount[8];

		Character* attackerCharacter = reinterpret_cast<Character*>(objects[attacker]);
		attackerCharacter->mKillCount++;
		FPP_LOG("User[%d] Die ", _id);
		cout << _id << "는 죽음\n";
		Die();

		for (int i = USER_START; i < MAX_USER; ++i)
		{
			auto character = reinterpret_cast<Character*>(objects[i]);

			character->state_lock.lock();
			if (Character::STATE::ST_INGAME == character->_state)
			{
				character->state_lock.unlock();
				userDeathcount[i] = character->mDeathCount;
				userKillcount[i] = character->mKillCount;
			}
			else {
				character->state_lock.unlock();
				userDeathcount[i] = -1;
				userKillcount[i] = -1;
			}
		}

		for (int i = USER_START; i < MAX_USER; ++i)
		{
			auto character = reinterpret_cast<Character*>(objects[i]);

			character->state_lock.lock();
			if (Character::STATE::ST_INGAME == character->_state)
			{
				character->state_lock.unlock();
				send_update_score_packet(character->_id, userDeathcount, userKillcount);
				send_kill_info_packet(character->_id, attackerCharacter->_id, _id);
			}
			else {
				character->state_lock.unlock();
			}
		}
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
	rx = 0.f, ry = 0.f, rz = 0.f, rw = 1.f;

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> RespawnControl(-50, 50);
	switch (RespawnSpot)
	{
	case 0:
		x = 17980.f + RespawnControl(rng), y = 1780.f + RespawnControl(rng), z = 100.f;
		break;
	case 1:
		x = 18350.f + RespawnControl(rng), y = 9610.f + RespawnControl(rng), z = 100.f;
		break;
	case 2:
		x = 18090.f + RespawnControl(rng), y = 17480.f + RespawnControl(rng), z = 100.f;
		break;
	case 3:
		x = 9730.f + RespawnControl(rng), y = 1620.f + RespawnControl(rng), z = 100.f;
		break;
	case 4:
		x = 9880.f + RespawnControl(rng), y = 18390.f + RespawnControl(rng), z = 100.f;
		break;
	case 5:
		x = 1990.f + RespawnControl(rng), y = 1740.f + RespawnControl(rng), z = 100.f;
		break;
	case 6:
		x = 1750.f + RespawnControl(rng), y = 10220.f + RespawnControl(rng), z = 100.f;
		break;
	case 7:
		x = 2200.f + RespawnControl(rng), y = 18140.f + RespawnControl(rng), z = 100.f;
		break;
	case 8:	{
		uniform_int_distribution<int> randomRespawn(0, 7);
		Respawn(randomRespawn(rng));
		break; 
		}
	}

	FPP_LOG("%d 플레이어 %d로 리스폰", this->_id, RespawnSpot);
}