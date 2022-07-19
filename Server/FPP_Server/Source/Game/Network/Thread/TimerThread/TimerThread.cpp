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
		//�̰� �������� ���ư� �� �־��� .
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
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_HEAL_RESPAWN)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_HEAL_RESPAWN);
						PostQueuedCompletionStatus(hiocp, 1, is_already.object_id, &wsa_ex->getWsaOver());
					}
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_PLAYER_RESPAWN)
					{
						//�̷� ������ �˻������� Ÿ�̸ӽ����忡�� �۾� if�� 1������..
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
						memcpy(wsa_ex->getBuf(), &is_already.x, sizeof(int));
						memcpy(wsa_ex->getBuf() + sizeof(int), &is_already.y, sizeof(int));
						memcpy(wsa_ex->getBuf() + sizeof(int) * 2, &is_already.z, sizeof(int));
						memcpy(wsa_ex->getBuf() + sizeof(int) * 3, &is_already.object_id, sizeof(int));
						memcpy(wsa_ex->getBuf() + sizeof(int) * 4, &is_already.player_id, sizeof(int));
						PostQueuedCompletionStatus(hiocp, 1, is_already.object_id, &wsa_ex->getWsaOver());
					}
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_GAME_WAIT)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_GAME_WAIT);
						PostQueuedCompletionStatus(hiocp, 1, NULL, &wsa_ex->getWsaOver());
					}
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_GAME_START)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_GAME_START);
						PostQueuedCompletionStatus(hiocp, 1, NULL, &wsa_ex->getWsaOver());
					}
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_GAME_END)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_GAME_END);
						PostQueuedCompletionStatus(hiocp, 1, NULL, &wsa_ex->getWsaOver());
					}
					else if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_GAME_RESET)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_GAME_RESET);
						PostQueuedCompletionStatus(hiocp, 1, NULL, &wsa_ex->getWsaOver());
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
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_HEAL_RESPAWN)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_HEAL_RESPAWN);
					PostQueuedCompletionStatus(hiocp, 1, exec_event.object_id, &wsa_ex->getWsaOver());
				}
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_PLAYER_RESPAWN)
				{
					//�̷� ������ �˻������� Ÿ�̸ӽ����忡�� �۾� if�� 1������..
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
					memcpy(wsa_ex->getBuf(), &exec_event.x, sizeof(int));
					memcpy(wsa_ex->getBuf() + sizeof(int), &exec_event.y, sizeof(int));
					memcpy(wsa_ex->getBuf() + sizeof(int) * 2, &exec_event.z, sizeof(int));
					memcpy(wsa_ex->getBuf() + sizeof(int) * 3, &exec_event.object_id, sizeof(int));
					memcpy(wsa_ex->getBuf() + sizeof(int) * 4, &exec_event.player_id, sizeof(int));
					PostQueuedCompletionStatus(hiocp, 1, exec_event.object_id, &wsa_ex->getWsaOver());
				}
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_GAME_WAIT)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_GAME_WAIT);
					PostQueuedCompletionStatus(hiocp, 1, NULL, &wsa_ex->getWsaOver());
				}
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_GAME_START)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_GAME_START);
					PostQueuedCompletionStatus(hiocp, 1, NULL, &wsa_ex->getWsaOver());
				}
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_GAME_END)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_GAME_END);
					PostQueuedCompletionStatus(hiocp, 1, NULL, &wsa_ex->getWsaOver());
				}
				else if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_GAME_RESET)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_GAME_RESET);
					PostQueuedCompletionStatus(hiocp, 1, NULL, &wsa_ex->getWsaOver());
				}
			}
			else {

				if (exec_event.exec_time > chrono::system_clock::now() + 200ms)
				{// ��ٷ��� �ϴ� �ð��� 0.2���̻��̶�� �ٸ� �� �߿��Ѱ� �� ���̿� ���� �� �����Ƿ�. 0.2�������� ��ٷ���. �׸��� �ٽ� ����.
					//���� 1�ʿ��µ�... �θ��� ���ݽð��� 0.2�ʷ� �������ֱ� ���� �̰͵� �ڿ�������... �پ���� ,,,
					timer_queue.push(exec_event);
					this_thread::sleep_for(200ms);
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