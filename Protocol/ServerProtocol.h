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
// 100~ 120 is Server - Server Number - Use this Number
// 125 - Ping Test
//-------------------------

const char PING_TEST = 125;
struct ping_test {
	unsigned char size;
	char type;
};

const char GL_PACKET_LOGIN = 100;

const char LG_PACKET_LOGIN_OK = 1;

//----------------------------------




const char LD_PACKET_LOGIN_AUTHOR = 1;
const char LD_PACKET_SIGNUP = 2;
const char LD_PACKET_REQUESTITEMINFO = 3;


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
//------------------- Lobbyserver to DBserver

struct dl_packet_login_author_ok {
	unsigned char size;
	char type;
	int playerid;
	char loginsuccess;			//1 ����, 0���� - ����
	int coin;
	short skintype;
	short playertype;
	char numberofplayerhaveitem;	//�÷��̾ �������ִ� �������� ����
	unsigned char itemcode[MAX_SHOP_ITEM];
};

struct dl_packet_signup_ok {
	unsigned char size;
	char type;
	int playerid;
	char loginsuccess;			//1 ����, 0���� - ����
};

struct dl_packet_getiteminfo {
	unsigned char size;
	char type;
	char MaxItemAmount;						//������ ���� �ִ� ������ ������ ����.
	unsigned char itemcode[MAX_SHOP_ITEM];	//MAX_SHOP_ITEM : �ִ�� ������ ������ �� �ִ� ������ ���� ����
	short price[MAX_SHOP_ITEM];
};
#pragma pack(pop)
