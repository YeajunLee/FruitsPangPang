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
	std::mutex db_lock;	//���� datarace�� �Ͼ�� �ȵǴ°͵�. coin/item���� ���... �Ѳ����� ��� ����.
	int mCoin;
	std::atomic_short mSkinType;
	bool bisAI;	// false - player , true - AI

	//�÷��̾ �����ϰ��ִ��� �ƴ����� �Ǵ��ϴ� ���𰡰� �־����.
	//�� ó���� DB���� �������̺��� �޾� �ְ�, ���� ����, �÷��̾��� ���¸� ���Ÿ� �����ֱ⶧����
	//�����ϴٰ� �����Ͽ� ����.
	concurrency::concurrent_unordered_map<int, bool> ShopInventory;	//<itemcode, have> �ش� �ڵ忡 �ش��ϴ� ������ �������̸� true, �ƴ϶�� false 
};

