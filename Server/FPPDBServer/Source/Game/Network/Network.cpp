#include <iostream>
#include "Network.h"
#include "../../DB/DB.h"
#include "../Server/Server.h"

using namespace std;


HANDLE hiocp;
SOCKET s_socket;
std::array<Server*, MAX_SERVER> servers;
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

void Disconnect(int c_id)
{
	auto server = reinterpret_cast<Server*>(servers[c_id]);
	server->state_lock.lock();
	closesocket(server->_socket);
	server->_state = Server::STATE::ST_FREE;
	server->state_lock.unlock();
}

void send_login_authorization_ok_packet(const int& server_id,const int& player_id, const char& succestype, const LoginInfo& info)
{
	auto server = reinterpret_cast<Server*>(servers[server_id]);
	dl_packet_login_author_ok packet;
	memset(&packet, 0, sizeof(dl_packet_login_author_ok));

	packet.size = sizeof(packet);
	packet.type = DL_PACKET_LOGIN_AUTHOR_OK;
	packet.playerid = player_id;
	packet.loginsuccess = succestype;
	packet.coin = info.p_coin;
	packet.skintype = info.p_skintype;
	packet.playertype = info.p_playertype;
	packet.numberofplayerhaveitem = info.p_numberofplayerhaveitem;
	memcpy(packet.itemcode, info.p_itemcode, sizeof(packet.itemcode));
	server->sendPacket(&packet, sizeof(packet));
}

void send_signup_ok_packet(const int& server_id, const int& player_id, const char& succestype)
{
	auto server = reinterpret_cast<Server*>(servers[server_id]);
	dl_packet_signup_ok packet;
	memset(&packet, 0, sizeof(dl_packet_signup_ok));

	packet.size = sizeof(packet);
	packet.type = DL_PACKET_SIGNUP_OK;
	packet.playerid = player_id;
	packet.loginsuccess = succestype;
	server->sendPacket(&packet, sizeof(packet));
}

void send_shop_data_packet(const int& server_id,dl_packet_getiteminfo& packet)
{
	auto server = reinterpret_cast<Server*>(servers[server_id]);

	packet.size = sizeof(packet);
	packet.type = DL_PACKET_GETITEMINFO;
	server->sendPacket(&packet, sizeof(packet));
}

void send_request_player_info(const int& server_id, const int& client_id, const LoginInfo& info)
{
	auto server = reinterpret_cast<Server*>(servers[server_id]);
	dg_packet_request_player_info packet;
	memset(&packet, 0, sizeof(packet));

	packet.size = sizeof(packet);
	packet.type = DG_PACKET_REQUEST_PLAYER_INFO;
	packet.skintype = info.p_skintype;
	packet.id = client_id;
	server->sendPacket(&packet, sizeof(packet));
}

void send_daily_reward(const int& server_id, const int& client_id, const char& succestype,const LoginInfo& info)
{
	auto server = reinterpret_cast<Server*>(servers[server_id]);
	dl_packet_daily_reward packet;
	memset(&packet, 0, sizeof(packet));

	packet.size = sizeof(packet);
	packet.type = DL_PACKET_DAILY_REWARD;
	packet.playerid = client_id;
	packet.rewardsuccess = succestype;
	packet.coin = info.p_coin;

	server->sendPacket(&packet, sizeof(packet));
}

void send_ping_test(const int& server_id)
{
	auto server = reinterpret_cast<Server*>(servers[server_id]);
	ping_test packet;
	memset(&packet, 0, sizeof(ping_test));

	packet.size = sizeof(packet);
	packet.type = PING_TEST;
	server->sendPacket(&packet, sizeof(packet));
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


void process_packet(int client_id, unsigned char* p)
{

	unsigned char packet_type = p[1];
	Server* server = servers[client_id];

	switch (packet_type) {
	case LD_PACKET_LOGIN_AUTHOR: {
		ld_packet_login_author* packet = reinterpret_cast<ld_packet_login_author*>(p);
		LoginInfo info{};
		cout << "서버 로그인 :" << server->_id << static_cast<int>(server->_state) << endl;
		cout << "패킷 사이즈 :" << packet->size  << "id : " << packet->id << "pass:"<<packet->pass << endl;
		char ret = Login(packet->id, packet->pass, info);
		send_login_authorization_ok_packet(client_id, packet->playerid, ret, info);
		break;
	}
	case LD_PACKET_SIGNUP: {
		ld_packet_signup* packet = reinterpret_cast<ld_packet_signup*>(p);
		char ret = SignUp(packet->id, packet->pass);
		send_signup_ok_packet(client_id, packet->playerid, ret);
		break;
	}
	case LD_PACKET_REQUESTITEMINFO: {
		ld_packet_requestiteminfo* packet = reinterpret_cast<ld_packet_requestiteminfo*>(p);
		dl_packet_getiteminfo iteminfo{};
		
		iteminfo.MaxItemAmount = GetShopData(iteminfo);
		send_shop_data_packet(client_id, iteminfo);
		break;
	}
	case LD_PACKET_BUYITEMUPDATE: {
		ld_packet_buyitemupdate* packet = reinterpret_cast<ld_packet_buyitemupdate*>(p);
		BuyItem(packet->id, packet->itemcode,packet->coin);
		break;
	}
	case LD_PACKET_UPDATE_SKINTYPE: {
		ld_packet_update_skintype* packet = reinterpret_cast<ld_packet_update_skintype*>(p);
		EquipItem(packet->id, packet->skintype);
		break;
	}
	case GD_PACKET_GET_PLAYER_INFO: {
		gd_packet_get_player_info* packet = reinterpret_cast<gd_packet_get_player_info*>(p);
		LoginInfo info{};
		GetPlayerInfo(packet->name, info);
		send_request_player_info(client_id, packet->id,info);
		break;
	}
	case GD_PACKET_UPDATE_PLAYER_INFO: {
		gd_packet_update_player_info* packet = reinterpret_cast<gd_packet_update_player_info*>(p);
		cout << "이름 : " << packet->name << "돈 지급:" << packet->coin << endl;
		UpdatePlayerInfo(packet->name, packet->coin);
		break;
	}
	case LD_PACKET_DAYPASS: {
		ld_packet_daypass* packet = reinterpret_cast<ld_packet_daypass*>(p);
		cout << "이름 : " << packet->id << "돈 지급: 100\n";
		LoginInfo info;
		int successtype = DailyReward(packet->id, info);
		send_daily_reward(client_id, packet->playerid, successtype, info);
		break;
	}
	}
}
