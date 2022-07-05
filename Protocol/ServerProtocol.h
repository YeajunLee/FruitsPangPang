#pragma once

//-------------------------
const short DBSERVER_PORT = 4200;
const int	GAMESERVER_START = 0;
const int	MAX_SERVER = 10;
const int  MAX_NAME_LEN = 21;
const int MAX_SHOP_ITEM = 20;
//------------------------

//-------------------------
// 1 ~ 99 is Client - Server Number   - Do Not Use This Number
// 100~ 120 is gameServer - LobbyServer Number - Use this Number
// 125 - Ping Test
//-------------------------

const char PING_TEST = 125;
struct ping_test {
	unsigned char size;
	char type;
};

const char GL_PACKET_LOGIN = 100;

const char LG_PACKET_LOGIN_OK = 1;
const char DG_PACKET_REQUEST_PLAYER_INFO = 2;
//----------------------------------




const char LD_PACKET_LOGIN_AUTHOR = 1;
const char LD_PACKET_SIGNUP = 2;
const char LD_PACKET_REQUESTITEMINFO = 3;
const char LD_PACKET_BUYITEMUPDATE = 4;
const char LD_PACKET_UPDATE_SKINTYPE = 5;
const char GD_PACKET_GET_PLAYER_INFO = 6;

const char DL_PACKET_LOGIN_AUTHOR_OK = 1;
const char DL_PACKET_SIGNUP_OK = 2;
const char DL_PACKET_GETITEMINFO = 3;


#pragma pack (push, 1)

//------------------- client to server

//------------------- Gameserver to Lobbyserver
struct gl_packet_login {
	unsigned char size;
	char type;
	short port;

};

//----------------- GameServer to DBServer
struct gd_packet_get_player_info {
	unsigned char size;
	char type;
	char name[MAX_NAME_LEN];
	int id;
};

//---------------- DBServer to GameServer

struct dg_packet_request_player_info {
	unsigned char size;
	char type;
	int id;
	short skintype;
};



//------------------- Lobbyserver to Gameserver
struct lg_packet_login_ok {
	unsigned char size;
	char type;

};

//------------------- Lobbyserver to DBserver

struct ld_packet_login_author {
	unsigned char size;
	char type;
	int playerid;
	char id[MAX_NAME_LEN];
	char pass[MAX_NAME_LEN];
};

struct ld_packet_signup {
	unsigned char size;
	char type;
	int playerid;
	char id[MAX_NAME_LEN];
	char pass[MAX_NAME_LEN];
};

struct ld_packet_requestiteminfo {
	unsigned char size;
	char type;
};

struct ld_packet_buyitemupdate {
	unsigned char size;
	char type;
	char id[MAX_NAME_LEN];
	unsigned char itemcode;
	int coin;
};

struct ld_packet_update_skintype {
	unsigned char size;
	char type;
	char id[MAX_NAME_LEN];
	short skintype;
};
//------------------- Lobbyserver to DBserver

struct dl_packet_login_author_ok {
	unsigned char size;
	char type;
	int playerid;
	char loginsuccess;			//1 성공, 0이하 - 실패
	int coin;
	short skintype;
	short playertype;
	char numberofplayerhaveitem;	//플레이어가 가지고있는 아이템의 갯수
	unsigned char itemcode[MAX_SHOP_ITEM];
};

struct dl_packet_signup_ok {
	unsigned char size;
	char type;
	int playerid;
	char loginsuccess;			//1 성공, 0이하 - 실패
};

struct dl_packet_getiteminfo {
	unsigned char size;
	char type;
	char MaxItemAmount;						//상점에 현재 있는 아이템 종류의 갯수.
	unsigned char itemcode[MAX_SHOP_ITEM];	//MAX_SHOP_ITEM : 최대로 상점에 들어가있을 수 있는 아이템 종류 갯수
	short price[MAX_SHOP_ITEM];
};
#pragma pack(pop)
