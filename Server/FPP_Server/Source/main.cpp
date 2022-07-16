#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "Game/Network/Network.h"
#include "Game/Network/Thread/WorkerThread/WorkerThread.h"
#include "Game/Network/Thread/TimerThread/TimerThread.h"
#include "Game/Network/Thread/LogThread/LogThread.h"
#include "Game/Object/Object.h"
#include "Game/Object/Interaction/Tree/Tree.h"
#include "Game/Object/Interaction/Punnet/Punnet.h"
#include "Game/Object/Interaction/Heal/Heal.h"
#include "Game/Object/Character/Character.h"
#include "Game/Server/Server.h"

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")

using namespace std;


int main(int argc, char* argv[])
{

	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	short server_port = -1;
	if(argc >1)
		server_port = atoi(argv[1]);

	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if (server_port != -1)
		server_addr.sin_port = htons(server_port);
	else
		server_addr.sin_port = htons(GAMESERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);
	cout <<	ntohs(server_addr.sin_port) << endl;
	hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(s_socket), hiocp, 0, 0);

	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	char	accept_buf[sizeof(SOCKADDR_IN) * 2 + 32 + 100];
	WSA_OVER_EX	accept_ex;
	*(reinterpret_cast<SOCKET*>(accept_ex.getBuf())) = c_socket;

	ZeroMemory(&accept_ex.getWsaOver(), sizeof(accept_ex.getWsaOver()));
	accept_ex.setCmd(CMD_ACCEPT);

	AcceptEx(s_socket, c_socket, accept_buf, 0, sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16, NULL, &accept_ex.getWsaOver());
	std::cout << "Accept Called\n";

	for (int i = 0; i < MAX_USER; ++i)
	{
		objects[i] = new Character();
		
	}

	for (int i = TREEID_START; i < TREEID_END; ++i)
	{
		if (i < TREEID_START + GREEN_TREE_CNT + 1 )
			objects[i] = new Tree(TREETYPE::GREEN);
		else
			objects[i] = new Tree(TREETYPE::ORANGE);
		auto tree = reinterpret_cast<Tree*>(objects[i]);
		tree->_id = i;
	}

	for (int i = PUNNETID_START; i < PUNNETID_END; ++i)
	{
		objects[i] = new Punnet();
		auto punnet = reinterpret_cast<Punnet*>(objects[i]);
		punnet->_id = i;
	}
	for (int i = HEALID_START; i < HEALID_END; ++i)
	{
		objects[i] = new Heal();
		auto heal = reinterpret_cast<Heal*>(objects[i]);
		heal->_id = i;
	}

	std::cout << "Creating Worker Threads\n";
	vector<thread> worker_threads;
	thread timer_thread{ TimerThread };
	thread logger_thread{ LogThread };
	for (int i = 0; i < 6; ++i)
		worker_threads.emplace_back(WorkerThread);


	//-----------------
	mServer = new Server();
	mServer->_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&mServer->server_addr, sizeof(mServer->server_addr));
	mServer->server_addr.sin_family = AF_INET;
	mServer->server_addr.sin_port = htons(LOBBYSERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &mServer->server_addr.sin_addr);
	mServer->wsa_ex_recv.getWsaBuf().buf = reinterpret_cast<char*>(mServer->wsa_ex_recv.getBuf());
	mServer->wsa_ex_recv.getWsaBuf().len = BUFSIZE;
	mServer->wsa_ex_recv.setCmd(CMD_SERVER_RECV);
	ZeroMemory(&mServer->wsa_ex_recv.getWsaOver(), sizeof(mServer->wsa_ex_recv.getWsaOver()));
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(mServer->_socket), hiocp, 1, 0);

	int rt = connect(mServer->_socket, reinterpret_cast<sockaddr*>(&mServer->server_addr), sizeof(mServer->server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		error_display(err_num);
		system("pause");
		//exit(0);
		closesocket(mServer->_socket);
		return false;
	}

	DWORD recv_flag = 0;
	int ret = WSARecv(mServer->_socket, &mServer->wsa_ex_recv.getWsaBuf(), 1, NULL, &recv_flag, &mServer->wsa_ex_recv.getWsaOver(), NULL);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}

	gl_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = GL_PACKET_LOGIN;
	packet.port = server_port;
	mServer->sendPacket(&packet, sizeof(packet));


	//--------------------------------------------


	//---------------
	mDBServer = new Server();
	mDBServer->_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&mDBServer->server_addr, sizeof(mDBServer->server_addr));
	mDBServer->server_addr.sin_family = AF_INET;
	mDBServer->server_addr.sin_port = htons(DBSERVER_PORT);
	//112.153.53.142
	inet_pton(AF_INET, "112.152.55.49", &mDBServer->server_addr.sin_addr);
	mDBServer->wsa_ex_recv.getWsaBuf().buf = reinterpret_cast<char*>(mDBServer->wsa_ex_recv.getBuf());
	mDBServer->wsa_ex_recv.getWsaBuf().len = BUFSIZE;
	mDBServer->wsa_ex_recv.setCmd(CMD_DBSERVER_RECV);
	ZeroMemory(&mDBServer->wsa_ex_recv.getWsaOver(), sizeof(mDBServer->wsa_ex_recv.getWsaOver()));
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(mDBServer->_socket), hiocp, 1, 0);

	rt = connect(mDBServer->_socket, reinterpret_cast<sockaddr*>(&mDBServer->server_addr), sizeof(mDBServer->server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		error_display(err_num);
		system("pause");
		//exit(0);
		closesocket(mDBServer->_socket);
		return false;
	}

	recv_flag = 0;
	ret = WSARecv(mDBServer->_socket, &mDBServer->wsa_ex_recv.getWsaBuf(), 1, NULL, &recv_flag, &mDBServer->wsa_ex_recv.getWsaOver(), NULL);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
	//---------




	for (auto& th : worker_threads)
		th.join();

	timer_thread.join();
	logger_thread.join();
	//스레드가 다 끝남. 그러니까 뮤텍스 필요없음.
	for (auto& object : objects) {
		if (!object->isPlayer()) break;
		auto character = reinterpret_cast<Character*>(object);
		//if (Character::STATE::ST_INGAME == character->_state)
			// Disconnect(character->_id);
	}

	for (auto& object : objects)
	{
		if(object)
			delete object;
	}
	closesocket(s_socket);
	WSACleanup();
}