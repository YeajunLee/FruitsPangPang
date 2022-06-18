#pragma once
#include <atomic>
#include <mutex>
#include "../Character.h"
#include "../../../Network/Network.h"
class Player : public Character
{
public:

	enum class STATE { ST_FREE, ST_ACCEPT, ST_INGAME };
	Player(OBJTYPE type = OBJTYPE::PLAYER, STATE state = STATE::ST_FREE);
	virtual ~Player();
	
	char name[MAX_NAME_SIZE];
	STATE	_state;
	std::mutex state_lock;
	std::atomic_bool _is_active;

public:
	WSA_OVER_EX wsa_ex_recv;
	SOCKET  _socket;
	int		_prev_size;
	void recvPacket();
	void PreRecvPacket(unsigned char* RemainMsg, int RemainBytes);
	void sendPacket(void* packet, int bytes);
public:
	std::mutex Coin_lock;
	int mCoin;
	std::atomic_short mSkinType;
	bool bisAI;	// false - player , true - AI
};

