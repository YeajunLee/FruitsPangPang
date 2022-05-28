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
//const int BUFSIZE = 256;

void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_Gamecallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
void CALLBACK recv_Lobbycallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
void CALLBACK recv_Aicallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

static std::shared_ptr<class Network> m_Network;


void send_login_packet(SOCKET& sock, const char& type);
void send_move_packet(SOCKET& sock, const float& x, const float& y, const float& z, struct FQuat& rotate, const float& value);
void send_spawnitemobj_packet(SOCKET& sock, const struct FVector& locate, const FRotator& rotate, const struct FVector& scale,
	const int& fruitType, const int& itemSlotNum, const int& uniqueid);//uniqueid is sync for banana
void send_getfruits_tree_packet(SOCKET& sock, const int& treeId);
void send_getfruits_punnet_packet(SOCKET& sock, const int& punnetId);
void send_useitem_packet(SOCKET& sock, const int& slotNum, const int& amount);
void send_hitmyself_packet(SOCKET& sock, const int& AttackerId, const int& FruitType = 0);
void send_change_hotkeyslot_packet(SOCKET& sock, const int& slotNum);
void send_pos_packet(SOCKET& sock, const float& x, const float& y, const float& z, const char& type);
void send_respawn_packet(SOCKET& sock, const char& WannaRespawn);
void send_PreGameSettingComplete_packet(SOCKET& sock);
void send_Cheat(SOCKET& sock, const char& cheatNum, const char& FruitType = 0);
void send_sync_banana(SOCKET& sock, const FVector& locate, const FRotator& rotate, const int& bananaid);
void send_match_request(SOCKET& sock);

class FRUITSPANGPANG_API Network : public std::enable_shared_from_this<Network>
{
private:
	WSADATA WSAData;
	void error_display(int err_no);
public:
	enum class AnimType
	{
		Throw,PickSword_GreenOnion,PickSword_Carrot,DropSword,Slash,Stab
	};
	class AMyCharacter* mMyCharacter;
	class AAICharacter* mAiCharacter[8];

	UPROPERTY()
	class AMyCharacter* mOtherCharacter[MAX_USER];
	class ATree* mTree[TREE_CNT];
	class APunnet* mPunnet[PUNNET_CNT];
	static std::shared_ptr<class Network> GetNetwork();
	int mSyncBananaID;	//바나나 Sync 맞추기용 Unique아이디	
	int mGeneratedID;
	short GameServerPort = -1;		//게임서버 접속용 port
	Network();
	~Network();
private:
	bool isInit;

public:
	const int getNewId();
	const int getNewBananaId();
	void release();
	bool init();
	void process_packet(unsigned char* p);
	void process_LobbyPacket(unsigned char* p);
	void process_Aipacket(int client_id, unsigned char* p);
};


void send_anim_packet(SOCKET& sock, Network::AnimType type);

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

