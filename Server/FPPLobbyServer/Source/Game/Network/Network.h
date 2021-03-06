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
void DisConnectClient(const int& clientid);
void DisConnectServer(const int& serverid);
int DisConnect(const int& clientid,WSAOVERLAPPED* command);
int Generate_ServerId();
void process_packet(int client_id, unsigned char* p);
void process_packet_for_Server(int client_id, unsigned char* p);
void process_packet_for_DB(unsigned char* p);

void send_login_authorization_packet(const int& player_id, const char* id, const char* pass);
void send_request_iteminfo_packet();
void send_buyitem_updateDB_packet(const char* name, const unsigned char& itemcode, const int& coin);
void send_updateDB_skintype_packet(const char* name, const short& skintype);

void send_login_ok_packet(const int& player_id, const char& succestype, const int& coin, const short& skintype);
void send_signup_packet(const int& player_id, const char* id, const char* pass);
void send_signup_ok_packet(const int& player_id, const char& succestype);
void send_enter_ingame_packet(const int& player_id,const short& server_port, const short& aiamount = -1);
void send_match_update_packet(const int& player_id, const int& player_cnt);
void send_buyitem_result_packet(const int& player_id, const int& remaincoin, const unsigned char& itemcode);
void send_equip_response_packet(const int& player_id, const unsigned char& itemcode);
void send_chat_packet(const int& player_id, const char* senderName, WCHAR* msg);

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
	unsigned char _buf[BUFSIZE]; //wsa_over_ex?? ?????????? ?????? ????, wsa_over_ex?? ?????????? 
	//workerthread???? _cmd?? ???????? ?????? ?????? ????. 
	int _serverID;	//?????? iocp_key?? ?????? ???????? ?????? ????. Accept?? ?? ?? CreateIoCompletionPort?? client???????? ?????????? ????.
	//?? ???? ???????? ???????? ???? ????. 
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
