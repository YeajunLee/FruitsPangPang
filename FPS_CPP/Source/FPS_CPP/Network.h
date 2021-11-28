// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <iostream>
#include <memory>
#include <WS2tcpip.h>
#include "../../../Protocol/protocol.h"
#include "CoreMinimal.h"

/**
 *
 */
const int BUFSIZE = 256;

static std::shared_ptr<class Network> m_Network;

class EXP_OVER {
public:
	WSAOVERLAPPED	_wsa_over;
	WSABUF			_wsa_buf;
	unsigned char	_my_buf[BUFSIZE];
public:
	EXP_OVER(char num_bytes, void* mess)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = reinterpret_cast<char*>(_my_buf);
		_wsa_buf.len = num_bytes;
		memcpy(_my_buf, mess, num_bytes);
	}
	EXP_OVER()
	{

	}

	~EXP_OVER()
	{

	}
};

class FPS_CPP_API Network : public std::enable_shared_from_this<Network>
{
	const char* SERVER_ADDR = "127.0.0.1";
	const short SERVER_PORT = 4000;
	SOCKET s_socket;
	SOCKADDR_IN server_addr;
	WSADATA WSAData;
	void error_display(int err_no);

public:
	enum class AnimType
	{
		Throw
	};
	class AMyCharacter* mMyCharacter;
	class AMyCharacter* mOtherCharacter[MAX_USER];
	int WorldCharacterCnt;
	int mId;
	static std::shared_ptr<class Network> GetNetwork();
	unsigned char buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	EXP_OVER recv_expover;
	int prev_size;
	WSABUF mybuf_r;
	Network();
	~Network();
	void release();
	bool init();
	void C_Send();
	void C_Recv();
	void send_login_packet();
	void send_move_packet(const float& x, const float& y, const float& z,struct FQuat& rotate,const float& value,const char& movetype);
	void send_anim_packet(AnimType type);
	void process_packet(unsigned char* p);
};
