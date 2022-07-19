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
	virtual void ResetServer() override;
	void ReCycleServer();
	int CurrentMatchingPlayerCnt;
	std::mutex CurrentMatchingPlayerCnt_lock;

	std::array<int, 8> CurrentMatchingPlayer;
	short mServerPort;
	bool isAIEntered;
	int SaveAI_id;
	bool Match(const int& player_id,const short& amount = 1);
	void AIMatch();

};

