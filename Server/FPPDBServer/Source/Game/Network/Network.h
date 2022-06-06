#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <thread>
#include "../../../../../Protocol/protocol.h"
#include "../../../../../Protocol/ServerProtocol.h"

extern HANDLE hiocp;
extern SOCKET s_socket;

extern std::array<class Server*, MAX_SERVER> servers;

void error_display(int err_no);

void send_login_authorization_ok_packet(const int& server_id,const int& player_id, const char& succestype, const int& coin, const short& skintype);

int Generate_ServerId();
void process_packet(int client_id, unsigned char* p);

enum COMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND,//Basic
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

struct LoginInfo {
	char p_name[21];		//최대 10글자 + null문자 1
	char p_password[21];
	int p_coin;
	short p_skintype;
};