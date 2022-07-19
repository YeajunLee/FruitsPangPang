#include <iostream>
#include <ppl.h>
#include <concurrent_unordered_map.h>
#include "Network.h"
#include "../Object/Object.h"
#include "../Object/Character/Character.h"
#include "../Object/Character/Player/Player.h"
#include "../Object/Character/Npc/Npc.h"
#include "../Server/Server.h"
#include "../Server/GameServer/GameServer.h"
#include "../Server/DBServer/DBServer.h"

using namespace std;

HANDLE hiocp;
SOCKET s_socket;
std::array<Object*, MAX_USER_LOBBY + MAX_NPC_LOBBY> objects;
std::array<Server*, MAX_SERVER> servers;
class DBServer* dbserver;
concurrency::concurrent_priority_queue <struct Timer_Event> timer_queue;

WSA_OVER_EX::WSA_OVER_EX(COMMAND_IOCP cmd, char bytes, void* msg)
	: _cmd(cmd)
{
	ZeroMemory(&_wsaover, sizeof(_wsaover));
	_wsabuf.buf = reinterpret_cast<char*>(_buf);
	_wsabuf.len = bytes;
	memcpy(_buf, msg, bytes);
}

WSA_OVER_EX::~WSA_OVER_EX()
{

}

WSA_OVER_ONLY_SERVER::~WSA_OVER_ONLY_SERVER()
{

}


void error_display(int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, 0);
	wcout << lpMsgBuf << endl;
	//while (true);
	LocalFree(lpMsgBuf);
}


int Generate_Id()
{
	static int g_id = 0;
	for (int i = 0; i < MAX_USER; ++i)
	{
		
		auto user = reinterpret_cast<Player*>(objects[i]);
		user->state_lock.lock();
		if (Player::STATE::ST_FREE == user->_state)
		{
			user->_state = Player::STATE::ST_ACCEPT;
			user->state_lock.unlock();
			return i;
		}
		user->state_lock.unlock();
	}
	cout << "Player is Over the MAX_USER" << endl;
	return -1;
}

void DisConnectClient(const int& clientid)
{
	//player disconnect
	auto player = reinterpret_cast<Player*>(objects[clientid]);
	closesocket(player->_socket);
	player->state_lock.lock();
	player->_state = Player::STATE::ST_FREE;
	player->bisAI = false;
	player->state_lock.unlock();
}

void DisConnectServer(const int& serverid)
{
	auto server = reinterpret_cast<Server*>(servers[serverid]);
	closesocket(server->_socket);
	server->state_lock.lock();
	server->_state = Server::STATE::ST_FREE;
	server->state_lock.unlock();
	server->ResetServer();
	cout << "���� ����" << serverid << endl;
}

int DisConnect(const int& clientid,WSAOVERLAPPED* overlapped)
{
	WSA_OVER_EX* wsa_ex = reinterpret_cast<WSA_OVER_EX*>(overlapped);
	switch (wsa_ex->getCmd())
	{
	case CMD_RECV:
	{
		//player disconnect
		DisConnectClient(clientid);
		break;
	}
	case CMD_SERVER_RECV:
	{
		//GameServer Disconnect
		WSA_OVER_ONLY_SERVER* server_wsa = reinterpret_cast<WSA_OVER_ONLY_SERVER*>(overlapped);
		DisConnectServer(server_wsa->getID());
		break;
	}
	}
	return 0;
}

int Generate_ServerId()
{
	static int g_serverid = 0;
	for (int i = 0; i < MAX_SERVER; ++i)
	{

		auto server = reinterpret_cast<Server*>(servers[i]);
		server->state_lock.lock();
		if (Server::STATE::ST_FREE == server->_state)
		{
			server->_state = Server::STATE::ST_USING;
			server->state_lock.unlock();
			return i;
		}
		server->state_lock.unlock();
	}
	cout << "Server ID is Over the MAX_SERVER" << endl;
	return -1;
}

void send_login_authorization_packet(const int& player_id, const char* id, const char* pass)
{
	ld_packet_login_author packet;
	memset(&packet, 0, sizeof(ld_packet_login_author));

	packet.size = sizeof(packet);
	packet.type = LD_PACKET_LOGIN_AUTHOR;
	packet.playerid = player_id;
	strcpy_s(packet.id, id);
	strcpy_s(packet.pass, pass);
	dbserver->sendPacket(&packet, sizeof(packet));
}

void send_request_iteminfo_packet()
{
	ld_packet_requestiteminfo packet;
	memset(&packet, 0, sizeof(ld_packet_requestiteminfo));

	packet.size = sizeof(packet);
	packet.type = LD_PACKET_REQUESTITEMINFO;
	dbserver->sendPacket(&packet, sizeof(packet));
}

void send_buyitem_updateDB_packet(const char* name, const unsigned char& itemcode,const int& coin)
{
	ld_packet_buyitemupdate packet{};
	packet.size = sizeof(packet);
	packet.type = LD_PACKET_BUYITEMUPDATE;
	strcpy_s(packet.id, name);
	packet.itemcode = itemcode;
	packet.coin = coin;
	dbserver->sendPacket(&packet, sizeof(packet));
}

void send_updateDB_skintype_packet(const char* name, const short& skintype)
{
	ld_packet_update_skintype packet{};
	packet.size = sizeof(packet);
	packet.type = LD_PACKET_UPDATE_SKINTYPE;
	strcpy_s(packet.id, name);
	packet.skintype = skintype;
	dbserver->sendPacket(&packet, sizeof(packet));
}

void send_login_ok_packet(const int& player_id, const char& succestype, const int& coin, const short& skintype)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	lc_packet_login_ok packet;
	memset(&packet, 0, sizeof(lc_packet_login_ok));

	packet.size = sizeof(packet);
	packet.type = LC_PACKET_LOGIN_OK;
	packet.id = player_id;
	strcpy_s(packet.name, player->name);
	packet.loginsuccess = succestype;
	packet.coin = coin;
	packet.skintype = skintype;
	int i = 0;
	for (auto& p : player->ShopInventory)
	{
		packet.haveitems[i] = p.first;
		++i;
	}
	packet.numberofitemshave = i;

	player->sendPacket(&packet, sizeof(packet));
}


void send_signup_packet(const int& player_id, const char* id, const char* pass)
{
	ld_packet_signup packet;
	memset(&packet, 0, sizeof(ld_packet_signup));

	packet.size = sizeof(packet);
	packet.type = LD_PACKET_SIGNUP;
	packet.playerid = player_id;
	strcpy_s(packet.id, id);
	strcpy_s(packet.pass, pass);
	dbserver->sendPacket(&packet, sizeof(packet));
}

void send_signup_ok_packet(const int& player_id, const char& succestype)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	lc_packet_signup_ok packet;
	memset(&packet, 0, sizeof(lc_packet_signup_ok));

	packet.size = sizeof(packet);
	packet.type = LC_PACKET_SIGNUP_OK;
	packet.loginsuccess = succestype;

	player->sendPacket(&packet, sizeof(packet));
}

void send_enter_ingame_packet(const int& player_id, const short& server_port, const short& aiamount)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	lc_packet_match_response packet;
	memset(&packet, 0, sizeof(lc_packet_match_response));

	packet.port = server_port;
	packet.size = sizeof(packet);
	packet.type = LC_PACKET_MATCH_RESPONSE;
	packet.playertype = player->bisAI;
	packet.aiamount = aiamount;
	player->sendPacket(&packet, sizeof(packet));
}

void send_match_update_packet(const int& player_id, const int& player_cnt)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	lc_packet_match_update packet;
	memset(&packet, 0, sizeof(lc_packet_match_update));

	packet.playercnt = player_cnt;
	packet.size = sizeof(packet);
	packet.type = LC_PACKET_MATCH_UPDATE;
	player->sendPacket(&packet, sizeof(packet));
}

void send_buyitem_result_packet(const int& player_id, const int& remaincoin,const unsigned char& itemcode)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	lc_packet_buyitem_result packet;
	memset(&packet, 0, sizeof(lc_packet_buyitem_result));

	packet.size = sizeof(packet);
	packet.type = LC_PACKET_BUYITEM_RESULT;
	packet.Coin = remaincoin;
	packet.itemcode = itemcode;
	player->sendPacket(&packet, sizeof(packet));
}

void send_equip_response_packet(const int& player_id, const unsigned char& itemcode)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	lc_packet_equip_response packet;
	memset(&packet, 0, sizeof(lc_packet_equip_response));

	packet.size = sizeof(packet);
	packet.type = LC_PACKET_EQUIP_RESPONSE;
	packet.itemcode = itemcode;
	player->sendPacket(&packet, sizeof(packet));
}

void process_packet(int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	Object* object = objects[client_id];

	switch (packet_type) {
	case CL_PACKET_LOGIN: {
		cl_packet_login* packet = reinterpret_cast<cl_packet_login*>(p);
		Player* character = reinterpret_cast<Player*>(object);
		strcpy_s(character->name, packet->name);
		send_login_authorization_packet(character->_id,packet->name, packet->password);
		//send_login_ok_packet(client_id);

		character->_state = Player::STATE::ST_INGAME;
		break;
	
	}
	case GL_PACKET_LOGIN: {
		//���Ӽ����� �ʱ⿡ player�� ��ϵ�����, login ��Ŷ�� �������ν�
		//��μ� ���Ӽ��� ������Ʈ Ǯ�� ��ϵǰ� �ȴ�.
		//���� character���� �ʿ��� ������ �����ؿ����� �Ѵ�.
		//���� character�� ����ְ� �ȴ�.
		// ������ io�� ��ϵ� key��(CompletionKey)�� �ٲܼ��� �ֳ�? ���� �� ����.
		//�׷� process_packet��ü���� ����ϰ� �ִ� client_id�� object[client_id]�� 
		//sync�� ���� �ʰ� �� ���̴�. ex) ���Ӽ����� key�� 1��ŭ �и��� �Ǹ�
		// object 0 ��°�� key�� 1���� ��������
		// ���⼭ client_id�� ���°� ���� 1�� �ͼ� 
		// object[0]�� �ƴ� object[1]�� �����ϰ� �Ǵ°�
		// �̰Ϳ� ���� �ذ����� ? 
		// �׳� �����ϰ� key�� ������, ������ ���õȳ��� ������ ���⼭ �ѹ� �� ĳ���� �ؼ� ���ش�?
		// �̰� ���������� idea�ε�. �����غ���.
		// completion key�� �ܼ��� ���޸� ���شٰ� �������� ���� �̾߱��̴�.
		// �� ���̵��� ������ workerThread���� key���� ���� prev_size�� ���� �ؼ� ���� �ȵǴ� ���. 
		// iocp_key�� ������� �ʰ�, wsaoverlapped Ȯ�� ����ü�� key���� �־ �������.
		// �̸� ���� �������� wsa_over_only_server�� �����, serverID���� ���� �ִ´�. 
		// ����� ������.

		gl_packet_login* packet = reinterpret_cast<gl_packet_login*>(p);
		Player* character = reinterpret_cast<Player*>(object);
		
		for (auto& server : servers)
		{
			GameServer* gameserver = reinterpret_cast<GameServer*>(server);
			if (gameserver->mServerPort == packet->port)
			{
				server->_socket = character->_socket;
				server->_prev_size = 0;
				server->wsa_server_recv.getWsaBuf().buf = reinterpret_cast<char*>(server->wsa_server_recv.getBuf());
				server->wsa_server_recv.getWsaBuf().len = BUFSIZE;
				server->wsa_server_recv.setID(server->_id);
				server->wsa_server_recv.setCmd(CMD_SERVER_RECV);	//���� �������� ��Ŷ�� ���� ó�� ���ش�.
				ZeroMemory(&server->wsa_server_recv.getWsaOver(), sizeof(server->wsa_server_recv.getWsaOver()));


				server->recvPacket();
				server->state_lock.lock();
				server->_state = Server::STATE::ST_MATHCING;
				server->state_lock.unlock();
				//server assign end

				//clear character
				character->state_lock.lock();
				character->_state = Player::STATE::ST_FREE;
				character->state_lock.unlock();
				character->_socket = NULL;
				//clear character end
				lg_packet_login_ok spacket;
				memset(&spacket, 0, sizeof(spacket));
				spacket.size = sizeof(spacket);
				spacket.type = LG_PACKET_LOGIN_OK;
				server->sendPacket(&spacket, sizeof(spacket));
			}
		}



		break;
	}
	case CL_PACKET_MATCH_REQUEST: {
		cl_packet_match_request* packet = reinterpret_cast<cl_packet_match_request*>(p);
		bool bAllServerNotActivate = true;
		short AmountOfTryMatchingPlayer = 1;

		if (-1 != packet->amount)	//ai�ϰ�� packet�� -1�� �ƴϴ�.
			AmountOfTryMatchingPlayer = packet->amount;

		for (auto& server : servers) {
			GameServer* gameserver = reinterpret_cast<GameServer*>(server);
			gameserver->state_lock.lock();
			if (gameserver->_state == Server::STATE::ST_MATHCING)
			{
				gameserver->state_lock.unlock();
				bAllServerNotActivate = false;	//���Ӽ����� 1���� ���������Ƿ� false 
				bool res = gameserver->Match(object->_id, AmountOfTryMatchingPlayer);
				if (!res)
				{
					cout << "��Ī���� ��Ī ��õ�\n";
					Timer_Event instq;
					instq.player_id = object->_id;
					instq.object_id = AmountOfTryMatchingPlayer;
					instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_REQUEST;
					instq.exec_time = chrono::system_clock::now() + 1000ms;

					timer_queue.push(instq);
				}
				else {
					cout << "��Ī����\n";
				}
				
			}
			else {
				gameserver->state_lock.unlock();
			}
		}

		//�ƹ� ���Ӽ����� �������� ������ ���� �ϳ� ����.
		if (bAllServerNotActivate)
		{

			int newid = Generate_ServerId();
			Server* server = servers[newid];	//state == USING
			server->_id = newid;
			GameServer* gameserver = reinterpret_cast<GameServer*>(server);
			gameserver->mServerPort = 4000 + newid;	//��Ʈ�ѹ�
			wchar_t tmp[20];
			_itow_s(gameserver->mServerPort, tmp, 10);
			ShellExecute(NULL, TEXT("open"), TEXT("../FPP_Server\\x64\\Debug\\FPP_Server.exe"), tmp, NULL, SW_SHOW);

			cout << "���� �������� ����. ��Ī ��õ�\n";
			{
				Timer_Event instq;
				instq.player_id = object->_id;
				instq.object_id = AmountOfTryMatchingPlayer;
				instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_REQUEST;
				instq.exec_time = chrono::system_clock::now() + 1000ms;

				timer_queue.push(instq);
			}
		}

		break;
	}
	case CL_PACKET_SIGNUP: {
		cl_packet_login* packet = reinterpret_cast<cl_packet_login*>(p);
		Player* character = reinterpret_cast<Player*>(object);
		send_signup_packet(character->_id, packet->name, packet->password);

		break;
	}
	case CL_PACKET_BUY: {

		cl_packet_buy* packet = reinterpret_cast<cl_packet_buy*>(p);
		Player* character = reinterpret_cast<Player*>(object);
		int remaincoin = 0;
		Npc* ShopNpc = reinterpret_cast<Npc*>(objects[NPC_ID_START_LOBBY]);
		character->db_lock.lock();
		if (false == character->ShopInventory[ShopNpc->Shop[packet->itemcode].first])
		{
			if (0 <= character->mCoin - ShopNpc->Shop[packet->itemcode].second)
			{
				character->mCoin -= ShopNpc->Shop[packet->itemcode].second;
				character->ShopInventory[ShopNpc->Shop[packet->itemcode].first] = true;
				//update characters DB in playerhaveitem
				send_buyitem_updateDB_packet(character->name, packet->itemcode,character->mCoin);
			}
		}
		remaincoin = character->mCoin;
		character->db_lock.unlock();

		send_buyitem_result_packet(character->_id, remaincoin,packet->itemcode);	//� �������� �����ߴ���, ������ ���� ������.

		break;
	}
	case CL_PACKET_EQUIP: {
		cl_packet_equip* packet = reinterpret_cast<cl_packet_equip*>(p);
		Player* character = reinterpret_cast<Player*>(object);
		character->mSkinType = packet->itemcode;
		send_equip_response_packet(client_id, packet->itemcode);
		send_updateDB_skintype_packet(character->name, character->mSkinType);
		break;
	}
	}
}




void process_packet_for_Server(int client_id, unsigned char* p)
{

	unsigned char packet_type = p[1];
	Server* server = servers[client_id];

	switch (packet_type) {
	case LG_PACKET_LOGIN_OK: {
		lg_packet_login_ok* packet = reinterpret_cast<lg_packet_login_ok*>(p);

		cout << client_id << "��° ���� ��\n";

		break;
	}
	case GL_PACKET_SERVER_RESET: {
		gl_packet_server_reset* packet = reinterpret_cast<gl_packet_server_reset*>(p);
		auto gameserver = reinterpret_cast<GameServer*>(server);
		gameserver->ReCycleServer();
		gameserver->state_lock.lock();
		gameserver->_state = Server::STATE::ST_MATHCING;
		gameserver->state_lock.unlock();
		break;
	}
	}
}

void process_packet_for_DB(unsigned char* p)
{
	unsigned char packet_type = p[1];
	DBServer* server = dbserver;

	switch (packet_type) {
	case DL_PACKET_LOGIN_AUTHOR_OK: {
		dl_packet_login_author_ok* packet = reinterpret_cast<dl_packet_login_author_ok*>(p);
		switch (packet->loginsuccess)
		{
		case 1: 
		{
			Player* character = reinterpret_cast<Player*>(objects[packet->playerid]);
			character->mSkinType = packet->skintype;
			character->db_lock.lock();
			character->mCoin = packet->coin;
			character->db_lock.unlock();
			character->bisAI = packet->playertype;	//���������� ���Ǵ� ����. Ŭ�󿡰Է� �Ѱ��� ������ ����.

			//---------------------------------------
			//�÷��̾� ���� �����۰��� ����.
			//ĳ���Ͱ� ������ ������ ��� ����.
			for (int i = 0; i < packet->numberofplayerhaveitem; ++i)
			{
				character->ShopInventory.insert(make_pair(static_cast<int>(packet->itemcode[i]),true));
			}
			//-----------------------------------------

			send_login_ok_packet(packet->playerid, packet->loginsuccess, packet->coin, packet->skintype);
			std::cout << "�α��� ���� ����id :" << character->name << endl;
			break;
		}
		default: {
			send_login_ok_packet(packet->playerid, packet->loginsuccess, -1, -1);
			std::cout << "�α��� ���� ���� �ڵ� :" << (int)packet->loginsuccess << endl;
		}
		}
		break;
	}
	case DL_PACKET_SIGNUP_OK: {
		dl_packet_signup_ok* packet = reinterpret_cast<dl_packet_signup_ok*>(p);
		switch (packet->loginsuccess)
		{
		case 1:
		{
			Player* character = reinterpret_cast<Player*>(objects[packet->playerid]);
			send_signup_ok_packet(packet->playerid, packet->loginsuccess);
			std::cout << "ȸ������ ���� ����id :" << character->name << endl;
			break;
		}
		default: {
			send_signup_ok_packet(packet->playerid, packet->loginsuccess);
			std::cout << "ȸ������ ���� ���� �ڵ� :" << (int)packet->loginsuccess << endl;
		}
		}
	}
	case DL_PACKET_GETITEMINFO: {
		dl_packet_getiteminfo* packet = reinterpret_cast<dl_packet_getiteminfo*>(p);
		auto npc = reinterpret_cast<Npc*>(objects[NPC_ID_START_LOBBY]);
		npc->NumberofItemsInStore = packet->MaxItemAmount;
		for (int i = 0; i < packet->MaxItemAmount; ++i)
		{
			npc->Shop[packet->itemcode[i]].first = packet->itemcode[i];
			npc->Shop[packet->itemcode[i]].second = packet->price[i];
		}
	}
	}
}
