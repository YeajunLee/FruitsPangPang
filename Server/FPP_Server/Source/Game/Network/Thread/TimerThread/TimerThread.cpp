#include <iostream>
#include <chrono>
#include <thread>
#include <concurrent_priority_queue.h>
#include "TimerThread.h"
#include "../../Network.h"
#include "../../../Object/Character/Character.h"

using namespace std;

void TimerThread()
{
	bool triger = false;
	Timer_Event is_already;
	while (true) {
		//뽑고 다음번에 돌아갈 것 넣어줌 .
		while (true) {
			if (triger)
			{

				if (is_already.exec_time <= chrono::system_clock::now())
				{
					if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_TREE_RESPAWN)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_TREE_RESPAWN);
						PostQueuedCompletionStatus(hiocp, 1, is_already.object_id, &wsa_ex->getWsaOver());
					}
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_PUNNET_RESPAWN)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_PUNNET_RESPAWN);
						PostQueuedCompletionStatus(hiocp, 1, is_already.object_id, &wsa_ex->getWsaOver());
					}
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_PLAYER_RESPAWN)
					{
						//이런 간단한 검사정도만 타이머스레드에서 작업 if문 1개정도..
						auto player = reinterpret_cast<Character*>(objects[is_already.player_id]);
						if (player->hp <= 0)
						{
							WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
							wsa_ex->setCmd(CMD_PLAYER_RESPAWN);
							PostQueuedCompletionStatus(hiocp, 1, is_already.player_id, &wsa_ex->getWsaOver());
						}

					}
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_DURIAN_DMG)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_DURIAN_DMG);
						memcpy(wsa_ex->getBuf(), &is_already.object_id, sizeof(int));
						memcpy(wsa_ex->getBuf() + sizeof(int), &is_already.player_id, sizeof(int));
						memcpy(wsa_ex->getBuf() + sizeof(int) + sizeof(int), &is_already.spare, sizeof(int));
						memcpy(wsa_ex->getBuf() + sizeof(int) * 3, &is_already.spare2, sizeof(char));
						PostQueuedCompletionStatus(hiocp, 1, is_already.object_id, &wsa_ex->getWsaOver());
					}
					triger = false;

				}
				continue;
			}
			Timer_Event exec_event;
			if (!timer_queue.try_pop(exec_event))
				break;
			if (exec_event.exec_time <= chrono::system_clock::now())
			{
				if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_TREE_RESPAWN)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_TREE_RESPAWN);
					PostQueuedCompletionStatus(hiocp, 1, exec_event.object_id, &wsa_ex->getWsaOver());
				}
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_PUNNET_RESPAWN)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_PUNNET_RESPAWN);
					PostQueuedCompletionStatus(hiocp, 1, exec_event.object_id, &wsa_ex->getWsaOver());
				}
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_PLAYER_RESPAWN)
				{
					//이런 간단한 검사정도만 타이머스레드에서 작업 if문 1개정도..
					auto player = reinterpret_cast<Character*>(objects[exec_event.player_id]);
					if (player->hp <= 0)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_PLAYER_RESPAWN);
						PostQueuedCompletionStatus(hiocp, 1, exec_event.player_id, &wsa_ex->getWsaOver());
					}

				}
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_DURIAN_DMG)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_DURIAN_DMG);
					memcpy(wsa_ex->getBuf(), &exec_event.object_id, sizeof(int));
					memcpy(wsa_ex->getBuf() + sizeof(int), &exec_event.player_id, sizeof(int));
					memcpy(wsa_ex->getBuf() + sizeof(int) + sizeof(int), &exec_event.spare, sizeof(int));
					memcpy(wsa_ex->getBuf() + sizeof(int) * 3, &exec_event.spare2, sizeof(char));
					PostQueuedCompletionStatus(hiocp, 1, exec_event.object_id, &wsa_ex->getWsaOver());
				}
			}
			else {

				if (exec_event.exec_time > chrono::system_clock::now() + 1000ms)
				{// 기다려야 하는 시간이 1초이상이라면 다른 더 중요한게 이 사이에 들어올 수 있으므로. 1초정도만 기다려줌. 그리고 다시 넣음.
					timer_queue.push(exec_event);
					this_thread::sleep_for(1000ms);
				}
				else {
					is_already = exec_event;
					triger = true;
					break;
				}

			}
		}
		this_thread::sleep_for(chrono::duration_cast<chrono::milliseconds>(is_already.exec_time - chrono::system_clock::now()));
	}
}