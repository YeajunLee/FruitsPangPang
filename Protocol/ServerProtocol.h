#pragma once

const int  MAX_SERVER = 10;

//-------------------------
// 1 ~ 99 is Client - Server Number   - Do Not Use This Number
// 100~ 199 is Server - Server Number - Use this Number
//-------------------------

const char GL_PACKET_LOGIN = 100;

const char LG_PACKET_LOGIN_OK = 1;

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

//-------------------- server to client
#pragma pack(pop)
