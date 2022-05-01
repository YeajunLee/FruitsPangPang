#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <chrono>
#include <mutex>
#include <string>
#include <concurrent_priority_queue.h>
#include <concurrent_queue.h>
#include "../../../../../Protocol/protocol.h"

extern HANDLE hiocp;
extern SOCKET s_socket;

extern std::array<class Object*, MAX_OBJECT> objects;
extern std::atomic_int loginPlayerCnt;
extern std::atomic_bool GameActive;
extern std::atomic_bool CheatGamePlayTime;
extern concurrency::concurrent_priority_queue <struct Timer_Event> timer_queue;
extern concurrency::concurrent_queue <struct Log> logger;

void error_display(int err_no);
int Generate_Id();
void Disconnect(int id);
void process_packet(int client_id, unsigned char* p);
void send_login_ok_packet(int player_id);
void send_move_packet(int player_id, int mover_id, float value);
void send_anim_packet(int player_id, int animCharacter_id, char animtype);
void send_throwfruit_packet(int thrower_character_id, int other_character_id,
	float rx, float ry, float rz, float rw,	//rotate
	float lx, float ly, float lz,	//location
	float sx, float sy, float sz,	//scale
	int fruittype	//item code
);
void send_update_inventory_packet(int player_id, short slotNum);
void send_update_interstat_packet(const int& player_id, const int& object_id, const bool& CanHarvest, const int& interactType, const int& FruitType = -1);

void send_remove_object_packet(int player_id, int removeCharacter_id);
void send_update_userstatus_packet(int player_id);
void send_die_packet(int player_id, int deadplayer_id);
void send_respawn_packet(int player_id, int respawner_id);
void send_update_score_packet(int player_id, short* userdeathcount, short* userkillcount);
void send_gamewaiting_packet(int player_id);
void send_gamestart_packet(int player_id);
void send_gameend_packet(int player_id);
void send_cheat_changegametime_packet(int player_id);

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


struct Timer_Event {

	enum class TIMER_TYPE
	{
		TYPE_TREE_RESPAWN, TYPE_PLAYER_RESPAWN,TYPE_PUNNET_RESPAWN,
		TYPE_DURIAN_DMG,
		TYPE_GAME_WAIT,TYPE_GAME_START,TYPE_GAME_END
	};
	int object_id;
	int player_id;
	int spare;
	char spare2;
	std::chrono::system_clock::time_point exec_time;
	TIMER_TYPE type;

	constexpr bool operator < (const Timer_Event& R) const
	{
		return (exec_time > R.exec_time);
	}
};

void FPP_LOG(const char* strLogTxt,...);

struct Log {
public:
	std::string logtxt;
};