#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <chrono>
#include <mutex>
#include "../../../../../Protocol/protocol.h"

extern HANDLE hiocp;
extern SOCKET s_socket;

extern std::array<class Object*, MAX_OBJECT> objects;
extern std::array<int, 3> GameServer;
void error_display(int err_no);
int Generate_Id();
void process_packet(int client_id, unsigned char* p);

void send_login_ok_packet(int player_id);


enum COMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND, //Basic
	CMD_TREE_RESPAWN, CMD_PUNNET_RESPAWN, CMD_PLAYER_RESPAWN, //Respawn
	CMD_DURIAN_DMG, //Damage
	CMD_GAME_WAIT, CMD_GAME_START, CMD_GAME_END //Game Cycle
};

class WSA_OVER_EX {
	WSAOVERLAPPED _wsaover;
	COMMAND_IOCP _cmd;
	WSABUF _wsabuf;
	unsigned char _buf[BUFSIZE];
public:
	WSA_OVER_EX() = default;
	WSA_OVER_EX(COMMAND_IOCP cmd, char bytes, void* msg);
	~WSA_OVER_EX();

	WSAOVERLAPPED& getWsaOver() { return _wsaover; }
	const COMMAND_IOCP getCmd() const { return _cmd; }
	void setCmd(const COMMAND_IOCP& cmd) { _cmd = cmd; }
	unsigned char* getBuf() { return _buf; }
	WSABUF& getWsaBuf() { return _wsabuf; }

};

class Network {
public:
	Network();
	virtual ~Network();
	WSA_OVER_EX wsa_ex_recv;
	SOCKET  _socket;
	int		_prev_size;

	virtual void init() = 0;
};