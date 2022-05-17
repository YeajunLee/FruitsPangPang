#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <chrono>
#include <mutex>
#include "../../../../../Protocol/protocol.h"
#include "../../../../../Protocol/ServerProtocol.h"

extern HANDLE hiocp;
extern SOCKET s_socket;

extern std::array<class Object*, MAX_OBJECT> objects;
extern std::array<class Server*, MAX_SERVER> servers;
void error_display(int err_no);
int Generate_Id();
int Generate_ServerId();
void process_packet(int client_id, unsigned char* p);
void process_packet_for_Server(int client_id, unsigned char* p);

void send_login_ok_packet(int player_id);


enum COMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND, CMD_SERVER_RECV,//Basic
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


class WSA_OVER_ONLY_SERVER {
	WSAOVERLAPPED _wsaover;
	COMMAND_IOCP _cmd;	
	WSABUF _wsabuf;
	unsigned char _buf[BUFSIZE]; //wsa_over_ex와 자리배치를 똑같이 하여, wsa_over_ex로 캐스팅하고 
	//workerthread에서 _cmd로 구분해도 문제가 없도록 한다. 
	int _serverID;	//서버는 iocp_key로 몇번째 서버인지 분간이 안됨. Accept를 할 때 CreateIoCompletionPort를 client기준으로 맞춰버렸기 때문.
	//몇 번째 서버인지 알려주기 위한 변수. 
public:
	WSA_OVER_ONLY_SERVER() = default;
	~WSA_OVER_ONLY_SERVER();

	WSAOVERLAPPED& getWsaOver() { return _wsaover; }
	const COMMAND_IOCP getCmd() const { return _cmd; }
	void setCmd(const COMMAND_IOCP& cmd) { _cmd = cmd; }
	unsigned char* getBuf() { return _buf; }
	WSABUF& getWsaBuf() { return _wsabuf; }
	const int getID() const { return _serverID; }
	void setID(const int& id) { _serverID = id; }

};
