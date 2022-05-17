#pragma once
#include <mutex>
#include "../Network/Network.h"
class Server
{

public:

	enum class STATE { ST_FREE, ST_USING };
	Server();
	virtual ~Server();

	STATE	_state;
	std::mutex state_lock;
	std::atomic_bool _is_active;

public:
	WSA_OVER_EX wsa_ex_recv;
	SOCKET  _socket;
	SOCKADDR_IN server_addr;
	int		_prev_size;
	int		_idoffset;	//client_id 에서 offset값을 빼준게 최종 id
	int		_id;
	void recvPacket();
	void PreRecvPacket(unsigned char* RemainMsg, int RemainBytes);
	void sendPacket(void* packet, int bytes);
};

