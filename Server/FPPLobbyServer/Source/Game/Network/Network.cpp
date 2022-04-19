#include <iostream>
#include "Network.h"
#include "../Object/Object.h"
#include "../Object/Character/Character.h"
#include "../Object/Character/Player/Player.h"

using namespace std;

HANDLE hiocp;
SOCKET s_socket;
std::array<Object*, MAX_OBJECT> objects;


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


int Generate_Id()
{
	static int g_id = 0;
	for (int i = 0; i < MAX_USER; ++i)
	{
		auto user = reinterpret_cast<Player*>(objects[i]);
		user->state_lock.lock();
		if (Player::STATE::ST_FREE == user->_state)
		{
			user->_state = Player::STATE::ST_ACCEPT;
			user->state_lock.unlock();
			return i;
		}
		user->state_lock.unlock();
	}
	cout << "Player is Over the MAX_USER" << endl;
	return -1;
}

void send_login_ok_packet(int player_id)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	sc_packet_login_ok packet;
	memset(&packet, 0, sizeof(sc_packet_login_ok));

	packet.id = player_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	player->sendPacket(&packet, sizeof(packet));
}

void process_packet(int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	Object* object = objects[client_id];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);
		Player* character = reinterpret_cast<Player*>(object);
		strcpy_s(character->name, packet->name);
		send_login_ok_packet(client_id);
		break;

	}
	}
}