#pragma once
#include <mutex>
#include <array>
#include "../Server.h"
class GameServer : public Server
{
public:
	GameServer();
	virtual ~GameServer();
public:
	int CurrentMatchingPlayerCnt;
	std::mutex CurrentMatchingPlayerCnt_lock;

	std::array<int, 8> CurrentMatchingPlayer;
	short mServerPort;
	bool Match(const int& player_id);
};

