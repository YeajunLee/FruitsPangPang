#pragma once
#include <atomic>
#include <mutex>
#include <ppl.h>
#include <concurrent_unordered_map.h>
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
	std::mutex db_lock;	//절대 datarace가 일어나면 안되는것들. coin/item보유 등등... 한꺼번에 묶어서 관리.
	int mCoin;
	std::atomic_short mSkinType;
	bool bisAI;	// false - player , true - AI

	//플레이어가 보유하고있는지 아닌지를 판단하는 무언가가 있어야함.
	//맨 처음에 DB에서 상점테이블을 받아 넣고, 삭제 없이, 플레이어의 상태를 갱신만 시켜주기때문에
	//적합하다고 생각하여 넣음.
	concurrency::concurrent_unordered_map<int, bool> ShopInventory;	//<itemcode, have> 해당 코드에 해당하는 아이템 보유중이면 true, 아니라면 false 
};

