#include <iostream>
#include "WorkerThread.h"
#include "../../Network.h"
#include "../../../Server/Server.h"

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
			Disconnect(client_id);
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
			WSA_OVER_EX* wsa_server = reinterpret_cast<WSA_OVER_EX*>(overlapped);

			Server* server = reinterpret_cast<Server*>(servers[client_id]);
			int To_Process_Bytes = bytes + server->_prev_size;
			unsigned char* packets = wsa_server->getBuf();	//wsa_ex == player->wsa_ex.buf

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
			server->PreRecvPacket(packets, To_Process_Bytes);
			server->recvPacket();
			break;
		}
		case CMD_PING_TEST: {
			Server* server = reinterpret_cast<Server*>(servers[client_id]);
			server->state_lock.lock();
			if (server->_state != Server::STATE::ST_FREE)
			{
				server->state_lock.unlock();
				send_ping_test(client_id);		
				Timer_Event instq;
				instq.server_id = client_id;
				instq.type = Timer_Event::TIMER_TYPE::TYPE_PING_TEST;
				instq.exec_time = chrono::system_clock::now() + 1000ms;
				timer_queue.push(instq);
				cout << "["<< client_id << "]핑테스트" << endl;
			}
			else {
				server->state_lock.unlock();
				cout << "핑테스트 취소" << endl;
			}


			delete wsa_ex;
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
			int new_id = Generate_ServerId();
			Server* server = reinterpret_cast<Server*>(servers[new_id]);
			server->_id = new_id;
			server->_state = Server::STATE::ST_DB;
			server->_prev_size = 0;
			server->wsa_server_recv.getWsaBuf().buf = reinterpret_cast<char*>(server->wsa_server_recv.getBuf());
			server->wsa_server_recv.getWsaBuf().len = BUFSIZE;
			server->wsa_server_recv.setCmd(CMD_RECV);
			ZeroMemory(&server->wsa_server_recv.getWsaOver(), sizeof(server->wsa_server_recv.getWsaOver()));
			server->_socket = c_socket;

			CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), hiocp, new_id, 0);
			server->recvPacket();

			Timer_Event instq;
			instq.server_id = new_id;
			instq.type = Timer_Event::TIMER_TYPE::TYPE_PING_TEST;
			instq.exec_time = chrono::system_clock::now() + 1000ms;
			timer_queue.push(instq);

			ZeroMemory(&wsa_ex->getWsaOver(), sizeof(wsa_ex->getWsaOver()));
			c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
			*(reinterpret_cast<SOCKET*>(wsa_ex->getBuf())) = c_socket;
			AcceptEx(s_socket, c_socket, wsa_ex->getBuf() + 8, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &wsa_ex->getWsaOver());
			break;
		}		
		}
	}
}
