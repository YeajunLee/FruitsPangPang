#include <iostream>
#include "Character.h"
#include "../../Network/Network.h"


using namespace std;

Character::Character(OBJTYPE type, STATE state)
	:_state(state)
	, _is_active(true)
	,maxhp(100),hp(maxhp)
{
	_type = type;
}

Character::~Character()
{

}



void Character::recvPacket()
{
	ZeroMemory(&wsa_ex_recv.getWsaOver(), sizeof(wsa_ex_recv.getWsaOver()));
	wsa_ex_recv.getWsaBuf().buf = reinterpret_cast<char*>(wsa_ex_recv.getBuf() + _prev_size);
	wsa_ex_recv.getWsaBuf().len = BUFSIZE - _prev_size;

	DWORD flags = 0;
	int ret = WSARecv(_socket, &wsa_ex_recv.getWsaBuf(), 1, 0, &flags, &wsa_ex_recv.getWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			error_display(err);
		}
	}
}

void Character::sendPacket(void* packet, int bytes)
{
	WSA_OVER_EX* wsa_ex = new WSA_OVER_EX(CMD_SEND, bytes, packet);
	int ret = WSASend(_socket, &wsa_ex->getWsaBuf(), 1, 0, 0, &wsa_ex->getWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			error_display(err);
		}
	}
}