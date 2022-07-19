#include <iostream>
#include "GameServer.h"
#include "../../Network/Network.h"
#include "../../Object/Character/Player/Player.h"

using namespace std;
GameServer::GameServer()
	:CurrentMatchingPlayerCnt(0)
	, mServerPort(0)
	, isAIEntered(false)
	, SaveAI_id(-1)
{

	for (auto& p : CurrentMatchingPlayer)
	{
		p = -1;
	}
}

GameServer::~GameServer()
{
}

void GameServer::ResetServer()
{
	Server::ResetServer();
	CurrentMatchingPlayerCnt = 0;
	mServerPort = 0;
	isAIEntered = false;
	SaveAI_id = -1;
	_id = -1;
	for (auto& p : CurrentMatchingPlayer)
	{
		p = -1;
	}
}

void GameServer::ReCycleServer()
{
	//portnum,id  aren't reset
	CurrentMatchingPlayerCnt = 0;
	isAIEntered = false;
	SaveAI_id = -1;
	for (auto& p : CurrentMatchingPlayer)
	{
		p = -1;
	}
}

bool GameServer::Match(const int& player_id, const short& amount)
{
	CurrentMatchingPlayerCnt_lock.lock();
	if (CurrentMatchingPlayerCnt + amount <= MAX_PLAYER_CONN)
	{
		CurrentMatchingPlayer[CurrentMatchingPlayerCnt] = player_id;
		//amount가 1이 아니라면 AI가 접속한것으로 간주.
		if (1 != amount)
		{
			SaveAI_id = player_id;	//AI ID기억
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
			cout << "매칭완\n";
			for (int i = 0; i < currentplayer; ++i)
			{
				if (isAIEntered)
					if (CurrentMatchingPlayer[i] == SaveAI_id)
						continue;
				send_enter_ingame_packet(CurrentMatchingPlayer[i], mServerPort);
				if (CurrentMatchingPlayer[i] == SaveAI_id)
					isAIEntered = true;	//AI는 딱 한번만 접속하게 끔
			}
		}
		else {
			CurrentMatchingPlayerCnt_lock.unlock();
		}

		for (int i = 0; i < currentplayer; ++i)
		{
			send_match_update_packet(CurrentMatchingPlayer[i], currentplayer);
		}

		if(1 == currentplayer)	//첫 번째 접속, 그러니까 게임서버가 매칭이 시작된 이후로, AI자동매칭을 넣어줌.
		{
			Timer_Event instq;
			instq.player_id = this->_id;
			instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_WAITING_TIMEOUT;
			instq.exec_time = chrono::system_clock::now() + 10000ms;		//10초간 대기하고 반응이없다면 AI넣어줌.

			timer_queue.push(instq);
		}

		return true;	//Match Succeed
	}
	else {
		CurrentMatchingPlayerCnt_lock.unlock();
		cout << "매칭 최대인원 초과. 매칭실패\n";
	}

	return false;	//Match Failed
}

void GameServer::AIMatch()
{
	int needAiNum = 0;
	int ai_ID = -1;
	for (auto obj : objects)
	{
		if (nullptr == obj) break;
		if (false == obj->isPlayer()) break;
		auto player = reinterpret_cast<Player*>(obj);
		if (player->bisAI)
		{
			player->state_lock.lock();
			if (Player::STATE::ST_INGAME == player->_state)
			{
				player->_state = Player::STATE::ST_INMATCHING;
				player->state_lock.unlock();
				ai_ID = player->_id;
			}
			else {
				player->state_lock.unlock();
			}

			break;
		}
	}
	if (-1 == ai_ID)
	{
		cout << "현재 접속한 AI가 없음. ai 매칭 실패 10초 후 재시도 \n";
		state_lock.lock();
		_state = Server::STATE::ST_MATHCING;
		state_lock.unlock();
		Timer_Event instq;
		instq.player_id = _id;
		instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_WAITING_TIMEOUT;
		instq.exec_time = chrono::system_clock::now() + 10000ms;		//다시 AI 매칭 시도.

		timer_queue.push(instq);
		return;
	}

	CurrentMatchingPlayerCnt_lock.lock();
	needAiNum = MAX_PLAYER_CONN - CurrentMatchingPlayerCnt;
	if (needAiNum < 1)	//Ai가 아무도 안들어와도 되는 경우 ( 아래 주석에 설명 )
	{
		CurrentMatchingPlayerCnt_lock.unlock();
		return;		
		//혹~시 말도 안되는 확률로 Match() 함수가 돌아가고 있는 도중에 Timer에 의해서 여기가 들어올수가 있기 때문에
		//예외처리를 해준다.
	}



	CurrentMatchingPlayer[CurrentMatchingPlayerCnt] = ai_ID;
	SaveAI_id = ai_ID;	//AI ID기억
	for (int i = CurrentMatchingPlayerCnt; i < CurrentMatchingPlayerCnt + needAiNum; ++i)
	{
		CurrentMatchingPlayer[i] = ai_ID;
	}
	//------------ ai관련 예외처리
	CurrentMatchingPlayerCnt += needAiNum;
	int currentplayer = CurrentMatchingPlayerCnt;

	if (CurrentMatchingPlayerCnt == MAX_PLAYER_CONN)
	{
		CurrentMatchingPlayerCnt_lock.unlock();

		cout << "매칭완\n";
		for (int i = 0; i < currentplayer; ++i)
		{
			if (isAIEntered)
				if (CurrentMatchingPlayer[i] == SaveAI_id)
					continue;
			//ai 매칭완 (needAiNum을 같이 보내줌)
			if (CurrentMatchingPlayer[i] == SaveAI_id)
			{
				send_enter_ingame_packet(CurrentMatchingPlayer[i], mServerPort, needAiNum);
				isAIEntered = true;	//AI는 딱 한번만 접속하게 끔
			}
			//player 매칭완 ( default 인게임 패킷을 보냄 )
			else
			{
				send_enter_ingame_packet(CurrentMatchingPlayer[i], mServerPort);
			}
		}
	}
	else {
		CurrentMatchingPlayerCnt_lock.unlock();
	}

}