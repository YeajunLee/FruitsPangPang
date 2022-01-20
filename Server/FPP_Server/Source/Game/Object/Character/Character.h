#pragma once
#include <atomic>
#include <mutex>
#include <unordered_set>
#include "../Object.h"
#include "../../Network/protocol.h"
#include "../../Network/Network.h"

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
	std::atomic_short hp;
	short maxhp;
};

