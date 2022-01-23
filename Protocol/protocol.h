#pragma once

const short SERVER_PORT = 4000;
const int WORLD_HEIGHT = 8;
const int WORLD_WIDTH = 8;
const int BUFSIZE = 256;
const int  MAX_NAME_SIZE = 20;
const int  MAX_USER = 8;
const int  TREEID_START = MAX_USER;
const int  TREEID_END = TREEID_START + 3;
const int  MAX_OBJECT = 100;
const char MOVE_FORWARD = 0;
const char MOVE_RIGHT = 1;

const char CS_PACKET_LOGIN = 1;
const char CS_PACKET_MOVE = 2;
const char CS_PACKET_TEST = 99;
const char CS_PACKET_DIR = 3;
const char CS_PACKET_ANIM = 4;
const char CS_PACKET_SPAWNOBJ = 5;
const char CS_PACKET_GETFRUITS = 6;
const char CS_PACKET_USEITEM = 7;

const char SC_PACKET_LOGIN_OK = 1;
const char SC_PACKET_MOVE = 2;
const char SC_PACKET_PUT_OBJECT = 3;
const char SC_PACKET_REMOVE_OBJECT = 4;
const char SC_PACKET_DIR = 5;
const char SC_PACKET_ANIM = 6;
const char SC_PACKET_SPAWNOBJ = 7;
const char SC_PACKET_UPDATE_INVENTORY = 8;
const char SC_PACKET_UPDATE_TREESTAT = 9;

#pragma pack (push, 1)
struct cs_packet_login {
	unsigned char size;
	char	type;
	char	name[MAX_NAME_SIZE];
};

struct cs_packet_move {
	unsigned char size;
	char	type;
	char	movetype;			//forward = 0 , right = 1
	float x, y, z;				//pos
	float rx, ry, rz, rw;		//rotate
	float value;				//scala
	char	direction;			// 0 : up,  1: down, 2:left, 3:right
};

struct cs_packet_dir {
	unsigned char size;
	char	type;
	float x, y, z;

};
struct cs_packet_test {
	unsigned char size;
	char type;
	float zPos;
};

struct cs_packet_anim {
	unsigned char size;
	char type;
	char animtype;
};

struct cs_packet_spawnobj {
	unsigned char size;
	char type;
	float rx, ry, rz, rw;	//rotate
	float lx, ly, lz;		//location
	float sx, sy, sz;		//scale
};

struct cs_packet_getfruits {
	unsigned char size;
	char type;
	int tree_id;
};

struct cs_packet_useitem {
	unsigned char size;
	char type;
	short slotNum;
	short Amount;
};


//-------------------- server to client
struct sc_packet_login_ok {
	unsigned char size;
	char type;
	int		id;
	short	x, y;
};

struct sc_packet_move {
	unsigned char size;
	char type;
	char movetype;
	int		id;
	float x, y, z;
	float rx, ry, rz, rw;
	float value;
	bool isValid;
};

struct sc_packet_dir {
	unsigned char size;
	char type;
	int		id;
	float x, y, z;
	bool isValid;
};

struct sc_packet_put_object {
	unsigned char size;
	char type;
	int id;
	short x, y;
	char object_type;
	char	name[MAX_NAME_SIZE];
};

struct sc_packet_remove_object {
	unsigned char size;
	char type;
	int id;
};

struct sc_packet_anim {
	unsigned char size;
	char type;
	int id;
	char animtype;
};

struct sc_packet_spawnobj {
	unsigned char size;
	char type;
	int id;
	float rx, ry, rz, rw;	//rotate
	float lx, ly, lz;		//location
	float sx, sy, sz;		//scale
};

struct sc_packet_update_inventory {
	unsigned char size;
	char type;
	short slotNum;	
	short itemCode;
	short itemAmount;
};

struct sc_packet_update_treestat {
	unsigned char size;
	char type;
	int treeNum;		//몇 번째 나무
	bool canHarvest;	//나무의 상태
	int	fruitType;		//열매의 타입
};
#pragma pack(pop)
