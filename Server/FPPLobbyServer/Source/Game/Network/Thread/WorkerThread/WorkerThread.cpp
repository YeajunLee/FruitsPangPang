#include <iostream>
#include "WorkerThread.h"
#include "../../Network.h"
#include "../../../Object/Character/Character.h"
#include "../../../Object/Character/Player/Player.h"
#include "../../../Server/Server.h"
#include "../../../Server/GameServer/GameServer.h"

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

			Player* player = reinterpret_cast<Player*>(objects[client_id]);
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
			player->PreRecvPacket(packets, To_Process_Bytes);
			player->recvPacket();
			break;
		}
		case CMD_SERVER_RECV: {
			if (bytes == 0) {
				//Disconnect(client_id);
				break;
			}
			WSA_OVER_ONLY_SERVER* wsa_server = reinterpret_cast<WSA_OVER_ONLY_SERVER*>(overlapped);

			Server* server = reinterpret_cast<Server*>(servers[wsa_server->getID()]);
			int To_Process_Bytes = bytes + server->_prev_size;
			unsigned char* packets = wsa_server->getBuf();	//wsa_ex == player->wsa_ex.buf

			if (To_Process_Bytes >= packets[0])
			{
				do {
					process_packet_for_Server(wsa_server->getID(), packets);
					To_Process_Bytes -= packets[0];
					packets += packets[0];
				} while ((To_Process_Bytes & ~0) && (To_Process_Bytes >= packets[0]));
			}
			//To_Process_Bytes != 0 이면 packets보다 큰지 확인해야 알 수 있지만
			//to_Process_Bytes가 packets보다 크면, To_Process_Bytes != 0인건 확실하므로, 이때는 필요없는 구문
			//뒷구문이 앞으로오는건 의미가 없음. 앞구문에서 성공하여 cmp를 한 번 만으로 끝낼 수 있는 기대효과
			server->PreRecvPacket(packets, To_Process_Bytes);
			server->recvPacket();
			break;
		}
		case CMD_DB_RECV: {
			if (bytes == 0) {
				//Disconnect(client_id);
				break;
			}
			WSA_OVER_ONLY_SERVER* wsa_server = reinterpret_cast<WSA_OVER_ONLY_SERVER*>(overlapped);

			Server* server = reinterpret_cast<Server*>(dbserver);
			int To_Process_Bytes = bytes + server->_prev_size;
			unsigned char* packets = wsa_server->getBuf();	//wsa_ex == player->wsa_ex.buf

			if (To_Process_Bytes >= packets[0])
			{
				do {
					process_packet_for_DB(packets);
					To_Process_Bytes -= packets[0];
					packets += packets[0];
				} while ((To_Process_Bytes & ~0) && (To_Process_Bytes >= packets[0]));
			}
			//To_Process_Bytes != 0 이면 packets보다 큰지 확인해야 알 수 있지만
			//to_Process_Bytes가 packets보다 크면, To_Process_Bytes != 0인건 확실하므로, 이때는 필요없는 구문
			//뒷구문이 앞으로오는건 의미가 없음. 앞구문에서 성공하여 cmp를 한 번 만으로 끝낼 수 있는 기대효과
			server->PreRecvPacket(packets, To_Process_Bytes);
			server->recvPacket();
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
			Player* player = reinterpret_cast<Player*>(objects[new_id]);
			player->_id = new_id;
			player->_state = Player::STATE::ST_ACCEPT;
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
		case CMD_MATCH_REQUEST: {
			int AmountOfTryMatchingPlayer = 1;
			memcpy(&AmountOfTryMatchingPlayer, wsa_ex->getBuf(), sizeof(int));
			for (auto& server : servers) {
				GameServer* gameserver = reinterpret_cast<GameServer*>(server);
				gameserver->state_lock.lock();
				if (gameserver->_state == Server::STATE::ST_MATHCING)
				{
					gameserver->state_lock.unlock();
					bool res = gameserver->Match(client_id, AmountOfTryMatchingPlayer);
					if (!res)
					{
						Timer_Event instq;
						instq.player_id = client_id;
						instq.object_id = AmountOfTryMatchingPlayer;
						instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_REQUEST;
						instq.exec_time = chrono::system_clock::now() + 1000ms;

						timer_queue.push(instq);
					}

				}
				else {
					gameserver->state_lock.unlock();
				}
			}
			delete wsa_ex;
			break;
		}
		}
	}
}
