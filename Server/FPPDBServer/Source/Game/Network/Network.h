#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <thread>
#include <concurrent_priority_queue.h>
#include "../../../../../Protocol/protocol.h"
#include "../../../../../Protocol/ServerProtocol.h"

extern HANDLE hiocp;
extern SOCKET s_socket;

extern std::array<class Server*, MAX_SERVER> servers;
extern concurrency::concurrent_priority_queue <struct Timer_Event> timer_queue;

void error_display(int err_no);

void send_login_authorization_ok_packet(const int& server_id, const int& player_id, const char& succestype, const int& coin, const short& skintype);
void send_signup_ok_packet(const int& server_id, const int& player_id, const char& succestype);
void send_ping_test(const int& server_id);
void Disconnect(int id);
int Generate_ServerId();
void process_packet(int client_id, unsigned char* p);

struct Timer_Event {
	enum class TIMER_TYPE
	{
		TYPE_PING_TEST
	};
	int server_id;
	std::chrono::system_clock::time_point exec_time;
	TIMER_TYPE type;
	constexpr bool operator < (const Timer_Event& R) const
	{
		return (exec_time > R.exec_time);
	}
};

enum COMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND,//Basic
	CMD_PING_TEST	//PingTest
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