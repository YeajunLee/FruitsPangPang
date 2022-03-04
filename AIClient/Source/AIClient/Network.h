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

void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);


class WSA_OVER_EX {
	WSAOVERLAPPED _wsaover;
	WSABUF _wsabuf;
	unsigned char _OwnerId;
	unsigned char _buf[BUFSIZE];

public:
	WSA_OVER_EX() = default;
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
	const unsigned char getId() const { return _OwnerId; }
	void setId(const unsigned char& id) { _OwnerId = id; }

};

class AICLIENT_API Network : public std::enable_shared_from_this<Network>
{
	WSADATA WSAData;
	void error_display(int err_no);
public:
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
	int WorldCharacterCnt;
	int mGeneratedID;
public:
	const int getNewId();
	void release();
	bool init();
	void process_packet(unsigned char* p);
	void process_packet2(int client_id, unsigned char* p);
	void send_login_packet(SOCKET& sock);
	void send_move_packet(SOCKET& sock,const float& x, const float& y, const float& z, struct FQuat& rotate, const float& value);
	void send_spawnobj_packet(SOCKET& sock, const struct FVector& locate, const struct FQuat& rotate, const struct FVector& scale, const int& fruitType);
	void send_anim_packet(SOCKET& sock, AnimType type);
	void send_getfruits_tree_packet(SOCKET& sock, const int& treeId);
	void send_getfruits_punnet_packet(SOCKET& sock, const int& punnetId);
	void send_useitem_packet(SOCKET& sock, const int& slotNum, const int& amount);
	void send_hitmyself_packet(SOCKET& sock, const int& FruitType = 0);
	void send_change_hotkeyslot_packet(SOCKET& sock, const int& slotNum);
	void send_pos_packet(SOCKET& sock, const float& x, const float& y, const float& z, const char& type);
	
};
