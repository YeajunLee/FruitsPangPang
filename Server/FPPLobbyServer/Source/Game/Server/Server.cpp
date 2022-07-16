#include "Server.h"

Server::Server()
	:_prev_size(0)
	,_id(-1)
	,_state(Server::STATE::ST_FREE)
{
}

Server::~Server()
{
}

void Server::ResetServer()
{
}

void Server::recvPacket()
{
	ZeroMemory(&wsa_server_recv.getWsaOver(), sizeof(wsa_server_recv.getWsaOver()));
	wsa_server_recv.getWsaBuf().buf = reinterpret_cast<char*>(wsa_server_recv.getBuf() + _prev_size);
	wsa_server_recv.getWsaBuf().len = BUFSIZE - _prev_size;

	DWORD flags = 0;
	int ret = WSARecv(_socket, &wsa_server_recv.getWsaBuf(), 1, 0, &flags, &wsa_server_recv.getWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			error_display(err);
			DisConnectServer(_id);
		}
	}
}

void Server::PreRecvPacket(unsigned char* RemainMsg, int RemainBytes)
{
	_prev_size = 0;
	if (RemainBytes > 0)
	{
		_prev_size = RemainBytes;
		memcpy(wsa_server_recv.getBuf(), RemainMsg, RemainBytes);
	}
}

void Server::sendPacket(void* packet, int bytes)
{
	WSA_OVER_EX* wsa_ex = new WSA_OVER_EX(CMD_SEND, bytes, packet);
	int ret = WSASend(_socket, &wsa_ex->getWsaBuf(), 1, 0, 0, &wsa_ex->getWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			error_display(err);
			DisConnectServer(_id);
		}
	}
}
