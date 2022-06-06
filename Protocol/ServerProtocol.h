#pragma once

//-------------------------
const short DBSERVER_PORT = 4200;
const int	GAMESERVER_START = 0;
const int	MAX_SERVER = 10;
const int  MAX_NAME_LEN = 21;
//------------------------

//-------------------------
// 1 ~ 99 is Client - Server Number   - Do Not Use This Number
// 100~ 199 is Server - Server Number - Use this Number
//-------------------------

const char GL_PACKET_LOGIN = 100;

const char LG_PACKET_LOGIN_OK = 1;

//----------------------------------




const char LD_PACKET_LOGIN_AUTHOR = 1;



const char DL_PACKET_LOGIN_AUTHOR_OK = 1;


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

//------------------- Lobbyserver to DBserver

struct dl_packet_login_author_ok {
	unsigned char size;
	char type;
	int playerid;
	char loginsuccess;			//1 성공, 0이하 - 실패
	int coin;
	short skintype;
};


#pragma pack(pop)
