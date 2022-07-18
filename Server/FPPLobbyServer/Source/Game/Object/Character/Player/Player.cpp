#include "Player.h"
#include "../../../Network/Network.h"

Player::Player(OBJTYPE type, STATE state)
	:_state(state)
	, mCoin(0)
	, bisAI(false)
	, _prev_size(0)
	, _socket(INVALID_SOCKET)
	,name()
{
	_otype = type;
}

Player::~Player()
{

}


void Player::PreRecvPacket(unsigned char* RemainMsg, int RemainBytes)
{

	//player->_prev_size = 0;
	//ZeroMemory(wsa_ex->getBuf(), sizeof(wsa_ex->getBuf()));
	_prev_size = 0;
	if (RemainBytes > 0)
	{
		_prev_size = RemainBytes;
		memcpy(wsa_ex_recv.getBuf(), RemainMsg, RemainBytes);
	}
}

void Player::recvPacket()
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
			DisConnectClient(_id);
		}
	}
}

void Player::sendPacket(void* packet, int bytes)
{
	WSA_OVER_EX* wsa_ex = new WSA_OVER_EX(CMD_SEND, bytes, packet);
	int ret = WSASend(_socket, &wsa_ex->getWsaBuf(), 1, 0, 0, &wsa_ex->getWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			error_display(err);
			DisConnectClient(_id);
		}
	}
}