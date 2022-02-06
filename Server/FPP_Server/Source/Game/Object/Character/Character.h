#pragma once
#include <atomic>
#include <mutex>
#include <unordered_set>
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
	void sendPacket(void* packet, int bytes);

public:
	short maxhp;
	std::atomic_short hp;
	InventorySlot mSlot[4];

	void UpdateInventorySlotAtIndex(const int& index, FRUITTYPE itemcode, const int& amount);
	void Die();
};

