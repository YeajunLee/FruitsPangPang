#pragma once
#include <atomic>
#include <mutex>
#include "../Object.h"
#include "../../Network/Network.h"

struct InventorySlot {

	InventorySlot()
		:type(FRUITTYPE::NONE)
		,amount(0)
	{

	}
	FRUITTYPE type;
	short amount;
};

class Character : public Object
{
public:
	enum class STATE { ST_FREE, ST_ACCEPT, ST_INGAME };
	Character(OBJTYPE type = OBJTYPE::PLAYER, STATE state = STATE::ST_FREE);
	virtual ~Character();
	char name[MAX_NAME_SIZE];
	STATE	_state;
	std::mutex state_lock;
	std::atomic_bool _is_active;
public:
	WSA_OVER_EX wsa_ex_recv;
	SOCKET  _socket;
	int		_prev_size;
	void recvPacket();
	void PreRecvPacket(unsigned char* RemainMsg,int RemainBytes);
	void sendPacket(void* packet, int bytes);

public:
	bool bAi;
	short maxhp;
	std::atomic_short hp;
	InventorySlot mSlot[5];
	std::atomic_int mActivationSlot;
	std::atomic_int mKillCount;
	std::atomic_int mDeathCount;
	void UpdateInventorySlotAtIndex(const int& index, FRUITTYPE itemcode, const int& amount);
	void Die();
	void HurtBy(const int& damageCauserType, const int& attacker);
	void Hurt(const int& damage,const int& attacker);
	void Heal(const int& amount);
	void Respawn(const int& RespawnSpot);
};

