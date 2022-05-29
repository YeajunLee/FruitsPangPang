#pragma once

#include <atomic>
#include <mutex>
#include "../Network/Network.h"

class Server
{
public:

	enum class STATE { 
		ST_FREE,ST_MATHCING,ST_USING,	//GameServer
		ST_DB							//DB
	};
	Server();
	virtual ~Server();

	STATE	_state;
	std::mutex state_lock;
	std::atomic_bool _is_active;

public:
	WSA_OVER_ONLY_SERVER wsa_server_recv;
	SOCKET  _socket;
	int		_prev_size;
	int		_id;
	void recvPacket();
	void PreRecvPacket(unsigned char* RemainMsg, int RemainBytes);
	void sendPacket(void* packet, int bytes);
};

