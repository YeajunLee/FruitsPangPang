#include <iostream>
#include "WorkerThread.h"
#include "../../Network.h"
#include "../../../Object/Character/Character.h"
#include "../../../Object/Character/Player/Player.h"

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
			//To_Process_Bytes != 0 �̸� packets���� ū�� Ȯ���ؾ� �� �� ������
			//to_Process_Bytes�� packets���� ũ��, To_Process_Bytes != 0�ΰ� Ȯ���ϹǷ�, �̶��� �ʿ���� ����
			//�ޱ����� �����ο��°� �ǹ̰� ����. �ձ������� �����Ͽ� cmp�� �� �� ������ ���� �� �ִ� ���ȿ��
			player->PreRecvPacket(packets, To_Process_Bytes);
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
			Player* player = reinterpret_cast<Player*>(objects[new_id]);
			player->_id = new_id;
			player->_state = Player::STATE::ST_INGAME;
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
		}
	}
}