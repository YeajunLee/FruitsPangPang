#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>
#include "Game/Network/Network.h"
#include "Game/Network/Thread/WorkerThread/WorkerThread.h"
#include "Game/Network/Thread/TimerThread/TimerThread.h"
#include "Game/Object/Object.h"
#include "Game/Object/Character/Character.h"
#include "Game/Object/Character/Player/Player.h"
#include "Game/Object/Character/Npc/Npc.h"
#include "Game/Server/Server.h"
#include "Game/Server/GameServer/GameServer.h"
#include "Game/Server/DBServer/DBServer.h"

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")


using namespace std;


int main()
{
	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(LOBBYSERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);

	hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(s_socket), hiocp, 0, 0);//accept를 받기 위한 서버용 소켓을 열어둔다.

	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	char	accept_buf[sizeof(SOCKADDR_IN) * 2 + 32 + 100];
	WSA_OVER_EX	accept_ex;
	*(reinterpret_cast<SOCKET*>(accept_ex.getBuf())) = c_socket;

	ZeroMemory(&accept_ex.getWsaOver(), sizeof(accept_ex.getWsaOver()));
	accept_ex.setCmd(CMD_ACCEPT);

	AcceptEx(s_socket, c_socket, accept_buf, 0, sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16, NULL, &accept_ex.getWsaOver());
	std::cout << "Accept Called\n";

	for (int i = 0; i < MAX_USER_LOBBY; ++i)
	{
		objects[i] = new Player();

	}
	for (int i = NPC_ID_START_LOBBY; i < NPC_ID_END_LOBBY; ++i)
	{
		objects[i] = new Npc();
	}
	dbserver = new DBServer();
	for (int i = GAMESERVER_START; i < MAX_SERVER; ++i)
	{
		servers[i] = new GameServer();
	}
	std::cout << "Creating Worker Threads\n";
	vector<thread> worker_threads;
	thread timer_thread{ TimerThread };
	for (int i = 0; i < 6; ++i)
		worker_threads.emplace_back(WorkerThread);
	

	//----------
		//-----------------
	dbserver->_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&dbserver->server_addr, sizeof(dbserver->server_addr));
	dbserver->server_addr.sin_family = AF_INET;
	dbserver->server_addr.sin_port = htons(DBSERVER_PORT);

	// 112.152.55.49 . 112.153.53.142
	inet_pton(AF_INET, "112.152.55.49", &dbserver->server_addr.sin_addr);
	dbserver->wsa_server_recv.getWsaBuf().buf = reinterpret_cast<char*>(dbserver->wsa_server_recv.getBuf());
	dbserver->wsa_server_recv.getWsaBuf().len = BUFSIZE;
	dbserver->wsa_server_recv.setCmd(CMD_DB_RECV);
	ZeroMemory(&dbserver->wsa_server_recv.getWsaOver(), sizeof(dbserver->wsa_server_recv.getWsaOver()));
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(dbserver->_socket), hiocp, 1, 0);

	int rt = connect(dbserver->_socket, reinterpret_cast<sockaddr*>(&dbserver->server_addr), sizeof(dbserver->server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		error_display(err_num);
		system("pause");
		//exit(0);
		closesocket(dbserver->_socket);
		return false;
	}

	DWORD recv_flag = 0;
	int ret = WSARecv(dbserver->_socket, &dbserver->wsa_server_recv.getWsaBuf(), 1, NULL, &recv_flag, &dbserver->wsa_server_recv.getWsaOver(), NULL);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
	//--------
	send_request_iteminfo_packet();	//db에서 가져올것 가져오기.

	//send_login_authorization_packet(0, "ss", "s");

	for (auto& th : worker_threads)
		th.join();
	timer_thread.join();
	//스레드가 다 끝남. 그러니까 뮤텍스 필요없음.
	for (auto& object : objects) {
		if (!object->isPlayer()) break;
		auto character = reinterpret_cast<Character*>(object);
		//if (Character::STATE::ST_INGAME == character->_state)
			// Disconnect(character->_id);
	}

	for (auto& object : objects)
	{
		if (object)
			delete object;
	}
	closesocket(s_socket);
	WSACleanup();
}