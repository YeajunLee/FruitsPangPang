#include <iostream>
#include "WorkerThread.h"
#include "../../Network.h"
#include "../../../Object/Character/Character.h"
#include "../../../Object/Interaction/Tree/Tree.h"
#include "../../../Object/Interaction/Punnet/Punnet.h"

using namespace std;

void WorkerThread()
{
	for (;;) {
		DWORD bytes;
		LONG64 iocp_key;
		WSAOVERLAPPED* overlapped;
		BOOL ret = GetQueuedCompletionStatus(hiocp, &bytes, (PULONG_PTR)&iocp_key, &overlapped, INFINITE);
		int client_id = static_cast<int>(iocp_key);
		WSA_OVER_EX* wsa_ex = reinterpret_cast<WSA_OVER_EX*>(overlapped);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			std::cout << "GQCS Error";
			error_display(err_no);
			//Disconnect(client_id);
			if (wsa_ex->getCmd() == CMD_SEND)
				delete wsa_ex;
			continue;
		}

		switch (wsa_ex->getCmd()) {
		case CMD_RECV: {
			if (bytes == 0) {
				//Disconnect(client_id);
				break;
			}

			Character* player = reinterpret_cast<Character*>(objects[client_id]);
			int To_Process_Bytes = bytes + player->_prev_size;
			unsigned char* packets = wsa_ex->getBuf();	//wsa_ex == player->wsa_ex.buf

			if (To_Process_Bytes >= packets[0])
			{
				do {
					process_packet(client_id, packets);
					To_Process_Bytes -= packets[0];
					packets += packets[0];
				} while ((To_Process_Bytes & ~0) && (To_Process_Bytes >= packets[0]));
			}
			//To_Process_Bytes != 0 이면 packets보다 큰지 확인해야 알 수 있지만
			//to_Process_Bytes가 packets보다 크면, To_Process_Bytes != 0인건 확실하므로, 이때는 필요없는 구문
			//뒷구문이 앞으로오는건 의미가 없음. 앞구문에서 성공하여 cmp를 한 번 만으로 끝낼 수 있는 기대효과
			player->PreRecvPacket(packets,To_Process_Bytes);
			player->recvPacket();
			break;
		}

		case CMD_SEND: {
			//if (num_byte != wsa_ex->_wsa_buf.len) {
			//	Disconnect(client_id);
			//}
			delete wsa_ex;
			break;
		}
		case CMD_ACCEPT: {
			std::cout << "Accept Completed.\n";
			SOCKET c_socket = *(reinterpret_cast<SOCKET*>(wsa_ex->getBuf()));
			int new_id = Generate_Id();
			Character* player = reinterpret_cast<Character*>(objects[new_id]);
			player->_id = new_id;
			player->_state = Character::STATE::ST_INGAME;
			player->_prev_size = 0;
			player->wsa_ex_recv.getWsaBuf().buf = reinterpret_cast<char*>(player->wsa_ex_recv.getBuf());
			player->wsa_ex_recv.getWsaBuf().len = BUFSIZE;
			player->wsa_ex_recv.setCmd(CMD_RECV);
			ZeroMemory(&player->wsa_ex_recv.getWsaOver(), sizeof(player->wsa_ex_recv.getWsaOver()));
			player->_socket = c_socket;

			CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), hiocp, new_id, 0);
			player->recvPacket();

			ZeroMemory(&wsa_ex->getWsaOver(), sizeof(wsa_ex->getWsaOver()));
			c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
			*(reinterpret_cast<SOCKET*>(wsa_ex->getBuf())) = c_socket;
			AcceptEx(s_socket, c_socket, wsa_ex->getBuf() + 8, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &wsa_ex->getWsaOver());
			break;
		}
		case CMD_TREE_RESPAWN: {

			auto tree = reinterpret_cast<Tree*>(objects[client_id]);
			cout << "나무 id:" << client_id << endl;
			tree->canHarvest = true;
			tree->GenerateFruit();

			for (auto& other : objects)
			{
				if (!other->isPlayer()) break;
				auto player = reinterpret_cast<Character*>(other);
				if (player->_state == Character::STATE::ST_INGAME)
				{

					cout << "과일나무 생성됐다고 보냅니다" << endl;
					send_update_interstat_packet(other->_id, client_id - TREEID_START, true,INTERACT_TYPE_TREE, static_cast<int>(tree->_ftype));
				}
			}
			delete wsa_ex;
			break;
		}		
		case CMD_PUNNET_RESPAWN: {

			auto punnet = reinterpret_cast<Punnet*>(objects[client_id]);
			cout << "바구니 id:" << client_id << endl;
			punnet->canHarvest = true;
			punnet->GenerateFruit();

			for (auto& other : objects)
			{
				if (!other->isPlayer()) break;
				auto player = reinterpret_cast<Character*>(other);
				if (player->_state == Character::STATE::ST_INGAME)
				{

					cout << "과일바구니 생성됐다고 보냅니다" << endl;
					send_update_interstat_packet(other->_id, client_id - PUNNETID_START, true,INTERACT_TYPE_PUNNET, static_cast<int>(punnet->_ftype));
				}
			}
			delete wsa_ex;
			break;
		}
		case CMD_PLAYER_RESPAWN: {

			Character* RespawnPlayer = reinterpret_cast<Character*>(objects[client_id]);
			if (RespawnPlayer->hp > 0)
			{
				delete wsa_ex;
				break;
			}
			RespawnPlayer->Respawn(8);

			cout << "플레이어 " << client_id << "리스폰\n";

			for (auto& other : objects) {
				if (!other->isPlayer()) break;
				auto character = reinterpret_cast<Character*>(other);

				character->state_lock.lock();
				if (Character::STATE::ST_INGAME == character->_state)
				{
					character->state_lock.unlock();
					send_respawn_packet(character->_id, RespawnPlayer->_id);
					if (character->_id == RespawnPlayer->_id)
					{
						send_update_userstatus_packet(RespawnPlayer->_id);
					}
				}
				else character->state_lock.unlock();
			}

			delete wsa_ex;
			break;
		}
		case CMD_DURIAN_DMG: {

			int x{}, y{}, z{};
			int r = 300;
			char times{};
			memcpy(&x, wsa_ex->getBuf(), sizeof(int));
			memcpy(&y, wsa_ex->getBuf() + sizeof(int), sizeof(int));
			memcpy(&z, wsa_ex->getBuf() + sizeof(int) * 2, sizeof(int));
			memcpy(&times, wsa_ex->getBuf() + sizeof(int) * 3, sizeof(char));
			cout <<"위치 : 워커스레드"<<  x << "," << y << "," << z << "," << times << endl;
			if (times <= 0)
			{
				delete wsa_ex;
				break;
			}
			for (auto& other : objects)
			{
				if (!other->isPlayer())break;
				if ((other->x - x) * (other->x - x) + (other->y - y) * (other->y - y) + (other->z - z) * (other->z - z) <= r * r)
				{
					//lock하고 검사하는 것 보다, 범위로 쓸데없는 경우 먼저 짜르고 그다음에 lock하는게 더 빠를것이라 판단하여 이렇게 짰다.
					auto character = reinterpret_cast<Character*>(other);
					character->state_lock.lock();
					if (Character::STATE::ST_INGAME == character->_state)
					{
						character->state_lock.unlock();
						//character->Hurt(4);						
					}
					else character->state_lock.unlock();
					//dmg apply
				}
			}
			// timerThread
			if (0 < times - 1)
			{
				Timer_Event instq;
				instq.object_id = x;
				instq.player_id = y;
				instq.spare = z;
				instq.spare2 = times - 1;
				instq.type = Timer_Event::TIMER_TYPE::TYPE_DURIAN_DMG;
				instq.exec_time = chrono::system_clock::now() + 2000ms;
				timer_queue.push(instq);
			}
			delete wsa_ex;
			break;
		}
		}
	}
}