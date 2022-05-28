#include <iostream>
#include "GameServer.h"
#include "../../Network/Network.h"

GameServer::GameServer()
	:CurrentMatchingPlayerCnt(0)
	, mServerPort(0)
	, isAIEntered(false)
	, SaveAI_id(-1)
{
}

GameServer::~GameServer()
{
}

bool GameServer::Match(const int& player_id, const short& amount)
{
	CurrentMatchingPlayerCnt_lock.lock();
	if (CurrentMatchingPlayerCnt + amount <= 8)
	{
		CurrentMatchingPlayer[CurrentMatchingPlayerCnt] = player_id;
		//amount가 1이 아니라면 AI가 접속한것으로 간주.
		if (1 != amount)
		{
			SaveAI_id = player_id;
			for (int i = CurrentMatchingPlayerCnt; i < CurrentMatchingPlayerCnt + amount; ++i)
			{
				CurrentMatchingPlayer[i] = player_id;
			}
		}
		//------------ ai관련 예외처리
		CurrentMatchingPlayerCnt += amount;
		int currentplayer = CurrentMatchingPlayerCnt;

		if (CurrentMatchingPlayerCnt == MAX_PLAYER_CONN)
		{
			CurrentMatchingPlayerCnt_lock.unlock();

			state_lock.lock();
			_state = Server::STATE::ST_USING;
			state_lock.unlock();
			std::cout << "매칭완\n";
			for (int i = 0; i < currentplayer; ++i)
			{
				if (isAIEntered)
					if (CurrentMatchingPlayer[i] == SaveAI_id)
						continue;
				send_enter_ingame_packet(CurrentMatchingPlayer[i], mServerPort);
				if (CurrentMatchingPlayer[i] == SaveAI_id)
					isAIEntered = true;
			}
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
		std::cout << "매칭 최대인원 초과. 매칭실패\n";
	}

	return false;	//Match Failed
}
