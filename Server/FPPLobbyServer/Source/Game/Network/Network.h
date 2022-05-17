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
	unsigned char _buf[BUFSIZE]; //wsa_over_ex�� �ڸ���ġ�� �Ȱ��� �Ͽ�, wsa_over_ex�� ĳ�����ϰ� 
	//workerthread���� _cmd�� �����ص� ������ ������ �Ѵ�. 
	int _serverID;	//������ iocp_key�� ���° �������� �а��� �ȵ�. Accept�� �� �� CreateIoCompletionPort�� client�������� ������ȱ� ����.
	//�� ��° �������� �˷��ֱ� ���� ����. 
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
