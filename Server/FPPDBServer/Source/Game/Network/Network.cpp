#include <iostream>
#include "Network.h"
#include "../../DB/DB.h"
#include "../Server/Server.h"

using namespace std;


HANDLE hiocp;
SOCKET s_socket;
std::array<Server*, MAX_SERVER> servers;



WSA_OVER_EX::WSA_OVER_EX(COMMAND_IOCP cmd, char bytes, void* msg)
	: _cmd(cmd)
{
	ZeroMemory(&_wsaover, sizeof(_wsaover));
	_wsabuf.buf = reinterpret_cast<char*>(_buf);
	_wsabuf.len = bytes;
	memcpy(_buf, msg, bytes);
}

WSA_OVER_EX::~WSA_OVER_EX()
{

}

void error_display(int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, 0);
	wcout << lpMsgBuf << endl;
	//while (true);
	LocalFree(lpMsgBuf);
}

void send_login_authorization_ok_packet(const int& server_id,const int& player_id, const char& succestype, const int& coin, const short& skintype)
{
	auto server = reinterpret_cast<Server*>(servers[server_id]);
	dl_packet_login_author_ok packet;
	memset(&packet, 0, sizeof(dl_packet_login_author_ok));

	packet.size = sizeof(packet);
	packet.type = DL_PACKET_LOGIN_AUTHOR_OK;
	packet.playerid = player_id;
	packet.loginsuccess = succestype;
	packet.coin = coin;
	packet.skintype = skintype;
	server->sendPacket(&packet, sizeof(packet));
}

int Generate_ServerId()
{
	static int g_serverid = 0;
	for (int i = 0; i < MAX_SERVER; ++i)
	{

		auto server = reinterpret_cast<Server*>(servers[i]);
		server->state_lock.lock();
		if (Server::STATE::ST_FREE == server->_state)
		{
			server->_state = Server::STATE::ST_USING;
			server->state_lock.unlock();
			return i;
		}
		server->state_lock.unlock();
	}
	cout << "Server ID is Over the MAX_SERVER" << endl;
	return -1;
}


void process_packet(int client_id, unsigned char* p)
{

	unsigned char packet_type = p[1];
	Server* server = servers[client_id];

	switch (packet_type) {
	case LD_PACKET_LOGIN_AUTHOR: {
		ld_packet_login_author* packet = reinterpret_cast<ld_packet_login_author*>(p);
		LoginInfo info{};
		char ret = Login(packet->id, packet->pass, info);
		send_login_authorization_ok_packet(client_id,packet->playerid, ret, info.p_coin, info.p_skintype);
		break;
	}
	}
}
