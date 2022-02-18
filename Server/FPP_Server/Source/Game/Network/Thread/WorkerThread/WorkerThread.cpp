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
			unsigned char* packets = wsa_ex->getBuf();

			while (To_Process_Bytes >= packets[0]) {
				process_packet(client_id, packets);
				To_Process_Bytes -= packets[0];
				packets += packets[0];
				if (To_Process_Bytes <= 0)break;
			}
			player->_prev_size = 0;
			ZeroMemory(wsa_ex->getBuf(), sizeof(wsa_ex->getBuf()));

			if (To_Process_Bytes > 0) {

				player->_prev_size = To_Process_Bytes;
				memcpy(wsa_ex->getBuf(), packets, player->_prev_size);
			}
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
			for (auto& other : objects)
			{
				if (!other->isPlayer()) break;
				auto player = reinterpret_cast<Character*>(other);
				if (player->_state == Character::STATE::ST_INGAME)
				{
					auto tree = reinterpret_cast<Tree*>(objects[client_id]);
					cout << "나무 id:" << client_id << endl;
					tree->canHarvest = true;
					tree->GenerateFruit();

					cout << "과일나무 생성됐다고 보냅니다" << endl;
					send_update_treestat_packet(other->_id, client_id - TREEID_START, true, static_cast<int>(tree->_ftype));
				}
			}
			delete wsa_ex;
			break;
		}		
		case CMD_PUNNET_RESPAWN: {
			for (auto& other : objects)
			{
				if (!other->isPlayer()) break;
				auto player = reinterpret_cast<Character*>(other);
				if (player->_state == Character::STATE::ST_INGAME)
				{
					auto tree = reinterpret_cast<Punnet*>(objects[client_id]);
					cout << "바구니 id:" << client_id << endl;
					tree->canHarvest = true;
					//tree->GenerateFruit();

					cout << "과일바구니 생성됐다고 보냅니다" << endl;
					//send_update_treestat_packet(other->_id, client_id, true, static_cast<int>(tree->_ftype));
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
			RespawnPlayer->hp = RespawnPlayer->maxhp;
			RespawnPlayer->x = 32030, RespawnPlayer->y = 24480, RespawnPlayer->z = 398;
			RespawnPlayer->rx = 0, RespawnPlayer->ry = 0, RespawnPlayer->rz = 0, RespawnPlayer->rw = 1;

			cout << "플레이어 " << client_id << "리스폰\n";

			for (auto& other : objects) {
				if (!other->isPlayer()) break;
				auto character = reinterpret_cast<Character*>(other);

				character->state_lock.lock();
				if (Character::STATE::ST_INGAME == character->_state)
				{
					character->state_lock.unlock();
					send_respawn_packet(character->_id, RespawnPlayer->_id);
				}
				else character->state_lock.unlock();
			}

			delete wsa_ex;
			break;
		}
		}
	}
}