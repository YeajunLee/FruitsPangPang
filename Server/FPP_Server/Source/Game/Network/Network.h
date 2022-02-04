#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <chrono>
#include <mutex>
#include <concurrent_priority_queue.h>
#include "../../../../../Protocol/protocol.h"

extern HANDLE hiocp;
extern SOCKET s_socket;

extern std::array<class Object*, MAX_OBJECT> objects;

void error_display(int err_no);
int Generate_Id();
void Disconnect(int id);
void process_packet(int client_id, unsigned char* p);
void send_login_ok_packet(int player_id);
void send_move_packet(int player_id, int mover_id, float value, char movetype);
void send_anim_packet(int player_id, int animCharacter_id, char animtype);
void send_throwfruit_packet(int thrower_character_id, int other_character_id,
	float rx, float ry, float rz, float rw,	//rotate
	float lx, float ly, float lz,	//location
	float sx, float sy, float sz	//scale
);
void send_update_inventory(int player_id, short slotNum);
void send_update_treestat(int player_id, int object_id, bool CanHarvest, int FruitType = -1);

void send_dir_packet(bool isval, int player_id);
void send_remove_object(int player_id, int removeCharacter_id);
void send_update_userstatus(int player_id);




enum COMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND, CMD_TREE_RESPAWN
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
		TYPE_TREE_RESPAWN
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


extern concurrency::concurrent_priority_queue <Timer_Event> timer_queue;