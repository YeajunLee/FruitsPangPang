#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <chrono>
#include <mutex>
#include <concurrent_priority_queue.h>
#include "../../../../../Protocol/protocol.h"
#include "../../../../../Protocol/ServerProtocol.h"

extern HANDLE hiocp;
extern SOCKET s_socket;

extern std::array<class Object*, MAX_USER_LOBBY + MAX_NPC_LOBBY> objects;
extern std::array<class Server*, MAX_SERVER> servers;
extern class DBServer* dbserver;
extern concurrency::concurrent_priority_queue <struct Timer_Event> timer_queue;
void error_display(int err_no);
int Generate_Id();
int Generate_ServerId();
void process_packet(int client_id, unsigned char* p);
void process_packet_for_Server(int client_id, unsigned char* p);
void process_packet_for_DB(unsigned char* p);

void send_login_authorization_packet(const int& player_id, const char* id, const char* pass);
void send_request_iteminfo_packet();

void send_login_ok_packet(const int& player_id, const char& succestype, const int& coin, const short& skintype);
void send_signup_packet(const int& player_id, const char* id, const char* pass);
void send_signup_ok_packet(const int& player_id, const char& succestype);
void send_enter_ingame_packet(const int& player_id,const short& server_port);
void send_match_update_packet(const int& player_id, const int& player_cnt);
void send_buyitem_result_packet(const int& player_id, const int& remaincoin);

struct Timer_Event {
	enum class TIMER_TYPE 
	{
		TYPE_MATCH_REQUEST,
		TYPE_MATCH_WAITING_TIMEOUT
	};
	int object_id;
	int player_id;
	std::chrono::system_clock::time_point exec_time;
	TIMER_TYPE type;
	constexpr bool operator < (const Timer_Event& R) const
	{
		return (exec_time > R.exec_time);
	}
};

enum COMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND, CMD_SERVER_RECV,//Basic
	CMD_DB_RECV,	//DB
	CMD_GAME_WAIT, CMD_GAME_START, CMD_GAME_END, //Game Cycle
	CMD_MATCH_REQUEST, CMD_MATCH_WAITING_TIMEOUT	//Match
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
