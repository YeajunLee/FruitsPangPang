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
static std::shared_ptr<class Network> m_Network;
extern HANDLE hiocp;

void Worker_Thread();
void process_packet2(int client_id, unsigned char* p);

enum COMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND, //basic
	CMD_TREE_RESPAWN, CMD_PUNNET_RESPAWN, CMD_PLAYER_RESPAWN, //respawn
	CMD_DURIAN_DMG //Damage
};

class WSA_OVER_EX {
	WSAOVERLAPPED _wsaover;
	WSABUF _wsabuf;
	COMMAND_IOCP _cmd;
	unsigned char _buf[BUFSIZE];

public:
	WSA_OVER_EX() = default;
	WSA_OVER_EX(COMMAND_IOCP cmd, char bytes, void* msg)
		:_cmd(cmd)
	{
		ZeroMemory(&_wsaover, sizeof(_wsaover));
		_wsabuf.buf = reinterpret_cast<char*>(_buf);
		_wsabuf.len = bytes;
		memcpy(_buf, msg, bytes);
	}

	WSA_OVER_EX(char bytes, void* msg)
	{
		ZeroMemory(&_wsaover, sizeof(_wsaover));
		_wsabuf.buf = reinterpret_cast<char*>(_buf);
		_wsabuf.len = bytes;
		memcpy(_buf, msg, bytes);
	}
	~WSA_OVER_EX()
	{

	}

	WSAOVERLAPPED& getWsaOver() { return _wsaover; }
	unsigned char* getBuf() { return _buf; }
	WSABUF& getWsaBuf() { return _wsabuf; }
	const COMMAND_IOCP getCmd() const { return _cmd; }
	void setCmd(const COMMAND_IOCP& cmd) { _cmd = cmd; }

};

class AICLIENT_API Network : public std::enable_shared_from_this<Network>
{
	const char* SERVER_ADDR = "127.0.0.1";
	const short SERVER_PORT = 4000;
	SOCKET s_socket;
	SOCKADDR_IN server_addr;
	WSADATA WSAData;
	void error_display(int err_no);
public:
	WSA_OVER_EX recv_expover;
	int prev_size;
	int mId;
	static std::shared_ptr<class Network> GetNetwork();
private:
	bool isInit;
public:
	Network();
	~Network();

	enum class AnimType
	{
		Throw
	};
	class AAICharacter* mAiCharacter[8];
	class AMyCharacter* mOtherCharacter[MAX_USER];
	class ATree* mTree[10];
	class APunnet* mPunnet[10];
public:
	void release();
	bool init();
	void C_Recv();
	void process_packet(unsigned char* p);
	void send_login_packet();
	void send_move_packet(const float& x, const float& y, const float& z, struct FQuat& rotate, const float& value);
	void send_spawnobj_packet(const struct FVector& locate, const struct FQuat& rotate, const struct FVector& scale, const int& fruitType);
	void send_anim_packet(AnimType type);
	void send_getfruits_tree_packet(const int& treeId);
	void send_getfruits_punnet_packet(const int& punnetId);
	void send_useitem_packet(const int& slotNum, const int& amount);
	void send_hitmyself_packet(const int& FruitType = 0);
	void send_change_hotkeyslot_packet(const int& slotNum);
	void send_pos_packet(const float& x, const float& y, const float& z, const char& type);
	
};
