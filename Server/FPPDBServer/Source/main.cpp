#include <iostream>
#include <vector>
#include "Game/Network/Network.h"
#include "Game/Network/Thread/WorkerThread/WorkerThread.h"
#include "Game/Network/Thread/TimerThread/TimerThread.h"
#include "Game/Server/Server.h"
#include "DB/DB.h"

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")

using namespace std;

int main()
{
	setlocale(LC_ALL, "korean");
	wcout.imbue(locale("korean"));
	InitializeDB();
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(DBSERVER_PORT);
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

	for (int i = 0; i < MAX_SERVER; ++i)
	{
		servers[i] = new Server();
	}

	std::cout << "Creating Worker Threads\n";
	thread timer_thread{ TimerThread };
	vector<thread> worker_threads;
	for (int i = 0; i < 6; ++i)
		worker_threads.emplace_back(WorkerThread);


	for (auto& th : worker_threads)
		th.join();
	timer_thread.join();
	//스레드가 다 끝남. 그러니까 뮤텍스 필요없음.
	//for (auto& object : objects) {
	//	if (!object->isPlayer()) break;
	//	auto character = reinterpret_cast<Character*>(object);
	//	//if (Character::STATE::ST_INGAME == character->_state)
	//		// Disconnect(character->_id);
	//}

	//or (auto& object : objects)
	//
	//	if (object)
	//		delete object;
	//
	ReleaseDB();
	closesocket(s_socket);
	WSACleanup();
}