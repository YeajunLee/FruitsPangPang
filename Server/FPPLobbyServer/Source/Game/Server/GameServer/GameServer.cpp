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
		//amount�� 1�� �ƴ϶�� AI�� �����Ѱ����� ����.
		if (1 != amount)
		{
			SaveAI_id = player_id;	//AI ID���
			for (int i = CurrentMatchingPlayerCnt; i < CurrentMatchingPlayerCnt + amount; ++i)
			{
				CurrentMatchingPlayer[i] = player_id;
			}
		}
		//------------ ai���� ����ó��
		CurrentMatchingPlayerCnt += amount;
		int currentplayer = CurrentMatchingPlayerCnt;

		if (CurrentMatchingPlayerCnt == MAX_PLAYER_CONN)
		{
			CurrentMatchingPlayerCnt_lock.unlock();

			state_lock.lock();
			_state = Server::STATE::ST_USING;
			state_lock.unlock();
			cout << "��Ī��\n";
			for (int i = 0; i < currentplayer; ++i)
			{
				if (isAIEntered)
					if (CurrentMatchingPlayer[i] == SaveAI_id)
						continue;
				send_enter_ingame_packet(CurrentMatchingPlayer[i], mServerPort);
				if (CurrentMatchingPlayer[i] == SaveAI_id)
					isAIEntered = true;	//AI�� �� �ѹ��� �����ϰ� ��
			}
		}
		else {
			CurrentMatchingPlayerCnt_lock.unlock();
		}

		for (int i = 0; i < currentplayer; ++i)
		{
			send_match_update_packet(CurrentMatchingPlayer[i], currentplayer);
		}

		if(1 == currentplayer)	//ù ��° ����, �׷��ϱ� ���Ӽ����� ��Ī�� ���۵� ���ķ�, AI�ڵ���Ī�� �־���.
		{
			Timer_Event instq;
			instq.player_id = this->_id;
			instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_WAITING_TIMEOUT;
			instq.exec_time = chrono::system_clock::now() + 10000ms;		//10�ʰ� ����ϰ� �����̾��ٸ� AI�־���.

			timer_queue.push(instq);
		}

		return true;	//Match Succeed
	}
	else {
		CurrentMatchingPlayerCnt_lock.unlock();
		cout << "��Ī �ִ��ο� �ʰ�. ��Ī����\n";
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
		cout << "���� ������ AI�� ����. ai ��Ī ���� 10�� �� ��õ� \n";
		state_lock.lock();
		_state = Server::STATE::ST_MATHCING;
		state_lock.unlock();
		Timer_Event instq;
		instq.player_id = _id;
		instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_WAITING_TIMEOUT;
		instq.exec_time = chrono::system_clock::now() + 10000ms;		//�ٽ� AI ��Ī �õ�.

		timer_queue.push(instq);
		return;
	}

	CurrentMatchingPlayerCnt_lock.lock();
	needAiNum = MAX_PLAYER_CONN - CurrentMatchingPlayerCnt;
	if (needAiNum < 1)	//Ai�� �ƹ��� �ȵ��͵� �Ǵ� ��� ( �Ʒ� �ּ��� ���� )
	{
		CurrentMatchingPlayerCnt_lock.unlock();
		return;		
		//Ȥ~�� ���� �ȵǴ� Ȯ���� Match() �Լ��� ���ư��� �ִ� ���߿� Timer�� ���ؼ� ���Ⱑ ���ü��� �ֱ� ������
		//����ó���� ���ش�.
	}



	CurrentMatchingPlayer[CurrentMatchingPlayerCnt] = ai_ID;
	SaveAI_id = ai_ID;	//AI ID���
	for (int i = CurrentMatchingPlayerCnt; i < CurrentMatchingPlayerCnt + needAiNum; ++i)
	{
		CurrentMatchingPlayer[i] = ai_ID;
	}
	//------------ ai���� ����ó��
	CurrentMatchingPlayerCnt += needAiNum;
	int currentplayer = CurrentMatchingPlayerCnt;

	if (CurrentMatchingPlayerCnt == MAX_PLAYER_CONN)
	{
		CurrentMatchingPlayerCnt_lock.unlock();

		cout << "��Ī��\n";
		for (int i = 0; i < currentplayer; ++i)
		{
			if (isAIEntered)
				if (CurrentMatchingPlayer[i] == SaveAI_id)
					continue;
			//ai ��Ī�� (needAiNum�� ���� ������)
			if (CurrentMatchingPlayer[i] == SaveAI_id)
			{
				send_enter_ingame_packet(CurrentMatchingPlayer[i], mServerPort, needAiNum);
				isAIEntered = true;	//AI�� �� �ѹ��� �����ϰ� ��
			}
			//player ��Ī�� ( default �ΰ��� ��Ŷ�� ���� )
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