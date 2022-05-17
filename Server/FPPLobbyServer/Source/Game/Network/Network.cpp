#include <iostream>
#include "Network.h"
#include "../Object/Object.h"
#include "../Object/Character/Character.h"
#include "../Object/Character/Player/Player.h"
#include "../Server/Server.h"

using namespace std;

HANDLE hiocp;
SOCKET s_socket;
std::array<Object*, MAX_OBJECT> objects;
std::array<Server*, MAX_SERVER> servers;


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
void send_login_ok_packet(int player_id)
{
	auto player = reinterpret_cast<Player*>(objects[player_id]);
	sc_packet_login_ok packet;
	memset(&packet, 0, sizeof(sc_packet_login_ok));

	packet.id = player_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	player->sendPacket(&packet, sizeof(packet));
}

void process_packet(int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	Object* object = objects[client_id];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);
		Player* character = reinterpret_cast<Player*>(object);
		strcpy_s(character->name, packet->name);
		send_login_ok_packet(client_id);

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
		int newid = Generate_ServerId();
		Server* server = servers[newid];	//state == USING
		server->_id = newid;
		server->_socket = character->_socket;	
		server->_prev_size = 0;
		server->wsa_server_recv.getWsaBuf().buf = reinterpret_cast<char*>(server->wsa_server_recv.getBuf());
		server->wsa_server_recv.getWsaBuf().len = BUFSIZE;
		server->wsa_server_recv.setID(newid);
		server->wsa_server_recv.setCmd(CMD_SERVER_RECV);	//이제 서버관련 패킷은 따로 처리 해준다.
		ZeroMemory(&server->wsa_server_recv.getWsaOver(), sizeof(server->wsa_server_recv.getWsaOver()));

		server->recvPacket();
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
