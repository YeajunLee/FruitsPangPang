#include <iostream>
#include "Network.h"
#include "../Object/Object.h"
#include "../Object/Character/Character.h"
#include "../Object/Character/Player/Player.h"
#include "../Server/Server.h"
#include "../Server/GameServer/GameServer.h"
#include "../Server/DBServer/DBServer.h"

using namespace std;

HANDLE hiocp;
SOCKET s_socket;
std::array<Object*, MAX_OBJECT> objects;
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

void send_enter_ingame_packet(const int& player_id, const short& server_port)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	lc_packet_match_response packet;
	memset(&packet, 0, sizeof(lc_packet_match_response));

	packet.port = server_port;
	packet.size = sizeof(packet);
	packet.type = LC_PACKET_MATCH_RESPONSE;
	packet.playertype = player->bisAI;
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
		//게임서버는 초기에 player로 등록되지만, login 패킷을 보냄으로써
		//비로소 게임서버 오브젝트 풀에 등록되게 된다.
		//따라서 character에서 필요한 소켓은 복사해오도록 한다.
		//이후 character는 비워주게 된다.
		// 하지만 io에 등록된 key값(CompletionKey)는 바꿀수가 있나? 없는 것 같다.
		//그럼 process_packet자체에서 사용하고 있는 client_id와 object[client_id]의 
		//sync가 맞지 않게 될 것이다. ex) 게임서버로 key가 1만큼 밀리게 되면
		// object 0 번째의 key는 1번이 될테지만
		// 여기서 client_id로 오는것 또한 1이 와서 
		// object[0]이 아닌 object[1]을 참조하게 되는것
		// 이것에 대한 해결방법은 ? 
		// 그냥 무시하고 key를 보내고, 서버와 관련된놈은 어차피 여기서 한번 더 캐스팅 해서 없앤다?
		// 이거 나쁘지않은 idea인듯. 실행해보자.
		// completion key가 단순히 전달만 해준다고 가정했을 때의 이야기이다.
		// 윗 아이디어는 어차피 workerThread에서 key값을 통해 prev_size를 얻어내야 해서 말이 안되는 방법. 
		// iocp_key를 사용하지 않고, wsaoverlapped 확장 구조체에 key값을 넣어서 사용하자.
		// 이를 위해 서버전용 wsa_over_only_server를 만들고, serverID값을 따로 넣는다. 
		// 결과는 성공적.

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
				server->wsa_server_recv.setCmd(CMD_SERVER_RECV);	//이제 서버관련 패킷은 따로 처리 해준다.
				ZeroMemory(&server->wsa_server_recv.getWsaOver(), sizeof(server->wsa_server_recv.getWsaOver()));

				//GameServer* gameserver = reinterpret_cast<GameServer*>(server);
				//gameserver->mServerPort = 4100 + newid;	//포트넘버

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

		if (-1 != packet->amount)	//ai일경우 packet이 -1이 아니다.
			AmountOfTryMatchingPlayer = packet->amount;

		for (auto& server : servers) {
			GameServer* gameserver = reinterpret_cast<GameServer*>(server);
			gameserver->state_lock.lock();
			if (gameserver->_state == Server::STATE::ST_MATHCING)
			{
				gameserver->state_lock.unlock();
				bAllServerNotActivate = false;	//게임서버가 1개라도 켜져있으므로 false 
				bool res = gameserver->Match(object->_id, AmountOfTryMatchingPlayer);
				if (!res)
				{
					cout << "매칭실패 매칭 재시도\n";
					Timer_Event instq;
					instq.player_id = object->_id;
					instq.object_id = AmountOfTryMatchingPlayer;
					instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_REQUEST;
					instq.exec_time = chrono::system_clock::now() + 1000ms;

					timer_queue.push(instq);
				}
				else {

					cout << "매칭성공\n";
				}
				
			}
			else {
				gameserver->state_lock.unlock();
			}
		}

		//아무 게임서버도 켜져있지 않으면 새로 하나 켜줌.
		if (bAllServerNotActivate)
		{

			int newid = Generate_ServerId();
			Server* server = servers[newid];	//state == USING
			server->_id = newid;
			GameServer* gameserver = reinterpret_cast<GameServer*>(server);
			gameserver->mServerPort = 4000 + newid;	//포트넘버
			wchar_t tmp[20];
			_itow_s(gameserver->mServerPort, tmp, 10);
			ShellExecute(NULL, TEXT("open"), TEXT("../FPP_Server\\x64\\Debug\\FPP_Server.exe"), tmp, NULL, SW_SHOW);

			{
				Timer_Event instq;
				instq.player_id = server->_id;
				instq.type = Timer_Event::TIMER_TYPE::TYPE_MATCH_WAITING_TIMEOUT;
				instq.exec_time = chrono::system_clock::now() + 10000ms;		//10초간 대기하고 반응이없다면 AI넣어줌.

				timer_queue.push(instq);
			}

			cout << "서버 열린곳이 없음. 매칭 재시도\n";
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
	}
}




void process_packet_for_Server(int client_id, unsigned char* p)
{

	unsigned char packet_type = p[1];
	Server* server = servers[client_id];

	switch (packet_type) {
	case LG_PACKET_LOGIN_OK: {
		lg_packet_login_ok* packet = reinterpret_cast<lg_packet_login_ok*>(p);

		cout << client_id << "번째 접속 완\n";

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
			character->Coin_lock.lock();
			character->mCoin = packet->coin;
			character->Coin_lock.unlock();
			character->bisAI = packet->playertype;	//서버에서만 사용되는 변수. 클라에게로 넘겨줄 이유는 없다.

			send_login_ok_packet(packet->playerid, packet->loginsuccess, packet->coin, packet->skintype);
			std::cout << "로그인 성공 성공id :" << character->name << endl;
			break;
		}
		default: {
			send_login_ok_packet(packet->playerid, packet->loginsuccess, -1, -1);
			std::cout << "로그인 실패 실패 코드 :" << (int)packet->loginsuccess << endl;
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
			std::cout << "회원가입 성공 성공id :" << character->name << endl;
			break;
		}
		default: {
			send_signup_ok_packet(packet->playerid, packet->loginsuccess);
			std::cout << "회원가입 실패 실패 코드 :" << (int)packet->loginsuccess << endl;
		}
		}
	}
	}
}
