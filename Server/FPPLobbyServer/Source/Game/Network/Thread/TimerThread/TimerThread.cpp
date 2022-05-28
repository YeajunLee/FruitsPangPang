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
					if (is_already.type == Timer_Event::TIMER_TYPE::TYPE_MATCH_REQUEST)
					{
						WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
						wsa_ex->setCmd(CMD_MATCH_REQUEST);
						PostQueuedCompletionStatus(hiocp, 1, is_already.player_id, &wsa_ex->getWsaOver());
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
				if (exec_event.type == Timer_Event::TIMER_TYPE::TYPE_MATCH_REQUEST)
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->setCmd(CMD_MATCH_REQUEST);
					PostQueuedCompletionStatus(hiocp, 1, exec_event.player_id, &wsa_ex->getWsaOver());
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