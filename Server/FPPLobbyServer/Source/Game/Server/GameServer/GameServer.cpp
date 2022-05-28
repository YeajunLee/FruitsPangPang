#include <iostream>
#include "GameServer.h"
#include "../../Network/Network.h"

GameServer::GameServer()
	:CurrentMatchingPlayerCnt(0)
	, mServerPort(0)
{
}

GameServer::~GameServer()
{
}

bool GameServer::Match(const int& player_id)
{
	CurrentMatchingPlayerCnt_lock.lock();
	if (CurrentMatchingPlayerCnt < 8)
	{
		CurrentMatchingPlayer[CurrentMatchingPlayerCnt] = player_id;
		CurrentMatchingPlayerCnt++;
		int currentplayer = CurrentMatchingPlayerCnt;
		if (CurrentMatchingPlayerCnt == MAX_PLAYER_CONN)
		{
			CurrentMatchingPlayerCnt_lock.unlock();

			state_lock.lock();
			_state = Server::STATE::ST_USING;
			state_lock.unlock();
			std::cout << "¸ÅÄª¿Ï\n";
			for (int i = 0; i < currentplayer; ++i)
			{
				send_enter_ingame_packet(CurrentMatchingPlayer[i], mServerPort);
			}
			//for (const auto& playerid : CurrentMatchingPlayer)
			//{
			//	send_enter_ingame_packet(playerid, mServerPort);
			//}
		}
		else {
			CurrentMatchingPlayerCnt_lock.unlock();
		}

		for (int i = 0; i < currentplayer; ++i)
		{
			send_match_update_packet(CurrentMatchingPlayer[i], currentplayer);
		}


		return true;	//Match Succeed
	}
	else {
		CurrentMatchingPlayerCnt_lock.unlock();
	}

	return false;	//Match Failed
}
