#pragma once

const short GAMESERVER_PORT = 4000;
const short LOBBYSERVER_PORT = 4100;
const int MAX_PLAYER_CONN = 7;
const int ACTIVE_AI_CNT = 6;
const int BUFSIZE = 256;
const int GAMEPLAYTIME_MILLI = 600'000;
const int GAMEPLAYTIME_CHEAT_MILLI = 10'000;
const int  MAX_NAME_SIZE = 21;
const int USER_START = 0;
const int  MAX_USER = 8;
const int MAX_USER_LOBBY = 1000;
const int MAX_NPC_LOBBY = 1;
const int NPC_ID_START_LOBBY = MAX_USER_LOBBY;
constexpr int NPC_ID_END_LOBBY = NPC_ID_START_LOBBY + MAX_NPC_LOBBY;
const int TREE_CNT = 56;
const int GREEN_TREE_CNT = 41;
const int PUNNET_CNT = 11;
const int HEAL_CNT = 10;
const int  TREEID_START = MAX_USER;
const int  TREEID_END = TREEID_START + TREE_CNT;
const int PUNNETID_START = TREEID_END;
const int PUNNETID_END = PUNNETID_START + PUNNET_CNT;
const int HEALID_START = PUNNETID_END;
const int HEALID_END = HEALID_START + HEAL_CNT;
const int  MAX_OBJECT = 100;
const int PLAYER_HP = 20;
const int PLAYER_HAVE_ITEM_LOBBY = 20;

const char POS_TYPE_DURIAN = 1;

const char INTERACT_TYPE_TREE = 1;
const char INTERACT_TYPE_PUNNET = 2;
const char INTERACT_TYPE_HEAL = 3;

const char CS_PACKET_LOGIN = 1;
const char CS_PACKET_MOVE = 2;
const char CS_PACKET_ANIM = 3;
const char CS_PACKET_SPAWNITEMOBJ = 4;
const char CS_PACKET_GETFRUITS_PUNNET = 5;
const char CS_PACKET_GETFRUITS_TREE = 6;
const char CS_PACKET_USEITEM = 7;
const char CS_PACKET_HIT = 8;
const char CS_PACKET_CHANGE_HOTKEYSLOT = 9;
const char CS_PACKET_POS = 10;
const char CS_PACKET_SELECT_RESPAWN = 11;
const char CS_PACKET_PREGAMESETTINGCOMPLETE = 12;
const char CS_PACKET_SYNC_BANANA = 13;
const char CL_PACKET_MATCH_REQUEST = 14;
const char CL_PACKET_LOGIN = 15;
const char CL_PACKET_SIGNUP = 16;
const char CS_PACKET_GETFRUITS_HEAL = 17;
const char CL_PACKET_BUY = 18;
const char CL_PACKET_EQUIP = 19;

const char CS_PACKET_CHEAT = 50;
const char CHEAT_TYPE_GAMETIME = 0;
const char CHEAT_TYPE_GIVEITEM = 1;

const char SC_PACKET_LOGIN_OK = 1;
const char SC_PACKET_MOVE = 2;
const char SC_PACKET_PUT_OBJECT = 3;
const char SC_PACKET_REMOVE_OBJECT = 4;
const char SC_PACKET_ANIM = 6;
const char SC_PACKET_SPAWNOBJ = 7;
const char SC_PACKET_UPDATE_INVENTORY = 8;
const char SC_PACKET_UPDATE_INTERSTAT = 9;
const char SC_PACKET_UPDATE_USERSTATUS = 10;
const char SC_PACKET_DIE = 11;
const char SC_PACKET_RESPAWN = 12;
const char SC_PACKET_UPDATE_SCORE = 13;
const char SC_PACKET_GAMEWAITING = 14;
const char SC_PACKET_GAMESTART = 15;
const char SC_PACKET_GAMEEND = 16;
const char SC_PACKET_SYNC_BANANA = 17;
const char SC_PACKET_KILL_INFO = 18;

const char SC_PACKET_CHEAT_GAMETIME = 100;


const char LC_PACKET_LOGIN_OK = 1;
const char LC_PACKET_MATCH_RESPONSE = 2;
const char LC_PACKET_MATCH_UPDATE = 3;
const char LC_PACKET_SIGNUP_OK = 4;
const char LC_PACKET_BUYITEM_RESULT = 5;

#pragma pack (push, 1)
struct cs_packet_login {
	unsigned char size;
	char	type;
	char	name[MAX_NAME_SIZE];
	char	cType;				//character type 0: player, 1: Ai
};

struct cs_packet_move {
	unsigned char size;
	char	type;
	float x, y, z;				//pos
	float rx, ry, rz, rw;		//rotate
	float speed;				//scala
};

struct cs_packet_anim {
	unsigned char size;
	char type;
	char animtype;
};

struct cs_packet_spawnitemobj {
	unsigned char size;
	char type;
	float rx, ry, rz, rw;	//rotate
	float lx, ly, lz;		//location
	float sx, sy, sz;		//scale
	int fruitType;			//item code
	int itemSlotNum;		//inventory slot num
	int uniquebananaid;		//item id(using banana sync)
};

struct cs_packet_getfruits {
	unsigned char size;
	char type;
	int obj_id;
};

struct cs_packet_useitem {
	unsigned char size;
	char type;
	short slotNum;
	short Amount;
};

struct cs_packet_hit {
	unsigned char size;
	char type;
	int fruitType;
	int attacker_id;
};

struct cs_packet_change_hotkeyslot {
	unsigned char size;
	char type;
	int HotkeySlotNum;	//activated inventory slot num
};

struct cs_packet_pos {
	unsigned char size;
	char type;
	char useType;
	float x, y, z;	//pos
};

struct cs_packet_select_respawn {
	unsigned char size;
	char type;
	char numbering;
};

struct cs_packet_pregamesettingcomplete {
	unsigned char size;
	char type;
};

struct cs_packet_sync_banana {
	unsigned char size;
	char type;
	int bananaid;
	float rx, ry, rz, rw;	//rotate
	float lx, ly, lz;		//location
};

struct cs_packet_cheat {
	unsigned char size;
	char type;
	char cheatType;
	char itemType;
};
//-------------------- client to LobbyServer

struct cl_packet_login {
	unsigned char size;
	char	type;
	char	name[MAX_NAME_SIZE];
	char	password[MAX_NAME_SIZE];
};

struct cl_packet_match_request {
	unsigned char size;
	char type;
	short amount;		//AI Amount
};

struct cl_packet_signup {
	unsigned char size;
	char	type;
	char	name[MAX_NAME_SIZE];
	char	password[MAX_NAME_SIZE];
};

struct cl_packet_buy {
	unsigned char size;
	char	type;
	unsigned char itemcode;
};

struct cl_packet_equip {
	unsigned char size;
	char	type;
	char itemcode;
};
//-------------------- Gameserver to client
struct sc_packet_login_ok {
	unsigned char size;
	char type;
	int		id;
	char name[MAX_NAME_SIZE];
	char TreeFruits[TREE_CNT];
	char PunnetFruits[PUNNET_CNT];
	char HealFruits[HEAL_CNT];
};

struct sc_packet_move {
	unsigned char size;
	char type;
	int		id;
	float x, y, z;
	float rx, ry, rz, rw;
	float speed;
};

struct sc_packet_put_object {
	unsigned char size;
	char type;
	int id;
	float x, y, z;
	float rx, ry, rz, rw;
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
	int fruitType;			//item code
	int uniqueid;			//item id(using banana sync)
};

struct sc_packet_update_inventory {
	unsigned char size;
	char type;
	short slotNum;	
	short itemCode;
	short itemAmount;
};

struct sc_packet_update_interstat {
	unsigned char size;
	char type;
	char useType;		//Tree = 1, Punnet = 2
	int objNum;			//몇 번째 오브젝트인지
	bool canHarvest;	//오브젝트의 상태
	int	fruitType;		//열매의 타입
};

struct sc_packet_update_userstatus {
	unsigned char size;
	char type;
	short hp;
};

struct sc_packet_die {
	unsigned char size;
	char type;
	int	id;
};

struct sc_packet_respawn {
	unsigned char size;
	char type;
	int	id;
	float rx, ry, rz, rw;	//rotate
	float lx, ly, lz;		//location

};

struct sc_packet_update_score {
	unsigned char size;
	char type;
	int id;
	short characterkillcount[8];
	short characterdeathcount[8];
};

struct sc_packet_gamewaiting {
	unsigned char size;
	char type;
};

struct sc_packet_gamestart {
	unsigned char size;
	char type;
};

struct sc_packet_gameend {
	unsigned char size;
	char type;
};

struct sc_packet_sync_banana {
	unsigned char size;
	char type;
	int bananaid;
	float rx, ry, rz, rw;	//rotate
	float lx, ly, lz;		//location
};


struct sc_packet_cheat_gametime {
	unsigned char size;
	char type;
	int milliseconds;
};

struct sc_packet_kill_info {
	unsigned char size;
	char type;
	char Attacker[MAX_NAME_SIZE];
	char Victim[MAX_NAME_SIZE];
};

//--------------LobbyServer to Client

struct lc_packet_login_ok {
	unsigned char size;
	char type;
	int		id;
	char name[MAX_NAME_SIZE];
	char loginsuccess;			//1 성공, 0이하 - 실패
	int coin;
	short skintype;
	char numberofitemshave;		//아래변수의 index용
	unsigned char haveitems[PLAYER_HAVE_ITEM_LOBBY];	//플레이어가 보유중인 아이템의 코드
};

struct lc_packet_match_response {
	unsigned char size;
	char type;
	short port;
	short playertype;
};

struct lc_packet_match_update {
	unsigned char size;
	char type;
	int playercnt;
};

struct lc_packet_signup_ok {
	unsigned char size;
	char type;
	char loginsuccess;
};

struct lc_packet_buyitem_result {
	unsigned char size;
	char type;
	int Coin;
};
#pragma pack(pop)
