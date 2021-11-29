#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <thread>
#include <vector>
#include <mutex>
#include "../../Protocol/protocol.h"
using namespace std;
#pragma comment (lib, "WS2_32.LIB")
#pragma comment (lib, "MSWSock.LIB")

const int BUFSIZE = 256;

HANDLE g_h_iocp;
SOCKET g_s_socket;
void error_display(int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, 0);
	wcout << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}

enum COMP_OP { OP_RECV, OP_SEND, OP_ACCEPT };
class EXP_OVER {
public:
	WSAOVERLAPPED	_wsa_over;
	COMP_OP			_comp_op;
	WSABUF			_wsa_buf;
	unsigned char	_net_buf[BUFSIZE];
public:
	EXP_OVER(COMP_OP comp_op, char num_bytes, void* mess) : _comp_op(comp_op)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = reinterpret_cast<char*>(_net_buf);
		_wsa_buf.len = num_bytes;
		memcpy(_net_buf, mess, num_bytes);
	}

	EXP_OVER(COMP_OP comp_op) : _comp_op(comp_op) {}

	EXP_OVER()
	{
		_comp_op = OP_RECV;
	}

	~EXP_OVER()
	{
	}
};
enum STATE { ST_FREE, ST_ACCEPT, ST_INGAME };
class CLIENT {
public:
	char name[MAX_NAME_SIZE];
	int	   _id;
	float  x, y,z;
	float rx, ry, rz, rw;
	
	mutex state_lock;
	STATE _state;

	EXP_OVER _recv_over;
	SOCKET	_socket;
	int		_prev_size;
public:
	CLIENT()
		: _state(ST_FREE)
		, _prev_size(0)
	{
		x = 0;
		y = 0;
		_prev_size = 0;
	}

	~CLIENT()
	{
		closesocket(_socket);
	}

	void do_recv()
	{
		DWORD recv_flag = 0;
		ZeroMemory(&_recv_over._wsa_over, sizeof(_recv_over._wsa_over));
		_recv_over._wsa_buf.buf = reinterpret_cast<char*>(_recv_over._net_buf + _prev_size);
		_recv_over._wsa_buf.len = sizeof(_recv_over._net_buf) - _prev_size;
		int ret = WSARecv(_socket, &_recv_over._wsa_buf, 1, 0, &recv_flag, &_recv_over._wsa_over, NULL);
		if (SOCKET_ERROR == ret) {
			int error_num = WSAGetLastError();
			if (ERROR_IO_PENDING != error_num)
				error_display(error_num);
		}
	}

	void do_send(int num_bytes, void* mess)
	{
		EXP_OVER* ex_over = new EXP_OVER(OP_SEND, num_bytes, mess);
		WSASend(_socket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, NULL);
	}
};

array <CLIENT, MAX_USER> clients;

int get_new_id()
{
	static int g_id = 0;

	for (int i = 0; i < MAX_USER; ++i) {
		clients[i].state_lock.lock();
		if (ST_FREE == clients[i]._state) {
			clients[i]._state = ST_ACCEPT;
			clients[i].state_lock.unlock();
			return i;
		}
		clients[i].state_lock.unlock();
	}
	std::cout << "Maximum Number of Clients Overflow!!\n";
	return -1;
}

void send_login_ok_packet(int c_id)
{
	sc_packet_login_ok packet;
	packet.id = c_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	packet.x = clients[c_id].x;
	packet.y = clients[c_id].y;
	clients[c_id].do_send(sizeof(packet), &packet);
}

void send_move_packet(int c_id, int mover,float value,char movetype)
{
	sc_packet_move packet;
	packet.id = mover;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_MOVE;
	packet.movetype = movetype;
	packet.x = clients[mover].x;
	packet.y = clients[mover].y;
	packet.z = clients[mover].z;
	packet.rx = clients[mover].rx;
	packet.ry = clients[mover].ry;
	packet.rz = clients[mover].rz;
	packet.rw = clients[mover].rw;
	packet.value = value;
	packet.isValid = false;
	clients[c_id].do_send(sizeof(packet), &packet);
}

void send_anim_packet(int c_id,int thrower_character, char animtype)
{
	sc_packet_anim packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_ANIM;
	packet.id = thrower_character;
	packet.animtype = animtype;
	clients[c_id].do_send(sizeof(packet), &packet);
}

void send_dir_packet(bool isval,int c_id)
{
	sc_packet_dir packet;
	packet.isValid = isval;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_DIR;
	packet.x = clients[c_id].x;
	packet.y = clients[c_id].y;
	packet.z = clients[c_id].z;
	clients[c_id].do_send(sizeof(packet), &packet);
}

void send_remove_object(int c_id, int victim)
{
	sc_packet_remove_object packet;
	packet.id = victim;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_REMOVE_OBJECT;
	clients[c_id].do_send(sizeof(packet), &packet);
}

void Disconnect(int c_id)
{
	clients[c_id].state_lock.lock();
	closesocket(clients[c_id]._socket);
	clients[c_id]._state = ST_FREE;
	clients[c_id].state_lock.unlock();
	for (auto& cl : clients) {
		cl.state_lock.lock();
		if (ST_INGAME != cl._state) {
			cl.state_lock.unlock();
			continue;
		}
		cl.state_lock.unlock();
		send_remove_object(cl._id, c_id);	//이거 언락위치 생각 
	}
}

void process_packet(int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	CLIENT& cl = clients[client_id];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);
		strcpy_s(cl.name, packet->name);
		send_login_ok_packet(client_id);

		for (auto& other : clients) {
			if (other._id == client_id) continue;
			other.state_lock.lock();
			if (ST_INGAME != other._state) {
				other.state_lock.unlock();
				continue;
			}
			other.state_lock.unlock();
			sc_packet_put_object packet;
			packet.id = client_id;
			strcpy_s(packet.name, cl.name);
			packet.object_type = 0;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.x = cl.x;
			packet.y = cl.y;
			other.do_send(sizeof(packet), &packet);
		}

		for (auto& other : clients) {
			if (other._id == client_id) continue;
			other.state_lock.lock();
			if (ST_INGAME != other._state) {
				other.state_lock.unlock();
				continue;
			}
			other.state_lock.unlock();
			sc_packet_put_object packet;
			packet.id = other._id;
			strcpy_s(packet.name, other.name);
			packet.object_type = 0;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.x = other.x;
			packet.y = other.y;
			cl.do_send(sizeof(packet), &packet);
		}
		break;
	}
	case CS_PACKET_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p);
		int x = cl.x;
		int y = cl.y;
		int z = cl.z;
		//switch (packet->direction) {
		//case 0: if (y > 0) y--; break;
		//case 1: if (y < (WORLD_HEIGHT - 1)) y++; break;
		//case 2: if (x > 0) x--; break;
		//case 3: if (x < (WORLD_WIDTH - 1)) x++; break;
		//default:
		//	std::cout << "Invalid move in client " << client_id << endl;
		//	exit(-1);
		//}
		cl.x = packet->x;
		cl.y = packet->y;
		cl.z = packet->z;
		cl.rx = packet->rx;
		cl.ry = packet->ry;
		cl.rz = packet->rz;
		cl.rw = packet->rw;
		
		for (auto& cl : clients) {
			cl.state_lock.lock();
			if (ST_INGAME == cl._state)
			{
				cl.state_lock.unlock();
				send_move_packet(cl._id, client_id,packet->value,packet->movetype);
			}
			else cl.state_lock.unlock();
		}
		break;
	}
	case CS_PACKET_DIR: {
		cs_packet_dir* packet = reinterpret_cast<cs_packet_dir*>(p);
		auto dx = abs(cl.x - packet->x);
		auto dy = abs(cl.y - packet->y);
		auto dz = abs(cl.z - packet->z);

		//if (dy > 4)
		//{
   		//	send_dir_packet(false, client_id);
		//}
		//else 
		{
			cl.x = packet->x;
			cl.y = packet->y; 
			cl.z = packet->z;
			send_dir_packet(true, client_id);
		}
		cout << dx << "," << dy << "," << dz << endl;
		break;
	}
	case CS_PACKET_ANIM: {
		cs_packet_anim* packet = reinterpret_cast<cs_packet_anim*>(p);

		for (auto& cl : clients) {
			if (cl._id == client_id) continue;
			cl.state_lock.lock();
			if (ST_INGAME == cl._state)
			{
				cl.state_lock.unlock();
				send_anim_packet(cl._id,client_id,packet->animtype);
			}
			else cl.state_lock.unlock();
		}

		break;
	}
	}
}

void worker()
{
	for (;;) {
		DWORD num_byte;
		LONG64 iocp_key;
		WSAOVERLAPPED* p_over;
		BOOL ret = GetQueuedCompletionStatus(g_h_iocp, &num_byte, (PULONG_PTR)&iocp_key, &p_over, INFINITE);
		//std::cout << "GQCS returned.\n";
		int client_id = static_cast<int>(iocp_key);
		EXP_OVER* exp_over = reinterpret_cast<EXP_OVER*>(p_over);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			std::cout << "GQCS Error";
			error_display(err_no);
			Disconnect(client_id);
			if (exp_over->_comp_op == OP_SEND)
				delete exp_over;
			continue;
		}
		switch (exp_over->_comp_op) {
		case OP_RECV: {
			CLIENT& cl = clients[client_id];
			int remain_data = num_byte + cl._prev_size;
			unsigned char* packet_start = exp_over->_net_buf;
			int packet_size = packet_start[0];

			while (packet_size <= remain_data) {
				process_packet(client_id, packet_start);
				remain_data -= packet_size;
				packet_start += packet_size;
				if (remain_data > 0) packet_size = packet_start[0];
				else break;
			}

			if (0 < remain_data) {
				cl._prev_size = remain_data;
				memcpy(&exp_over->_net_buf, packet_start, remain_data);
			}
			cl.do_recv();
			break;
		}
		case OP_SEND: {
			if (num_byte != exp_over->_wsa_buf.len) {
				// DISCONNECT();
			}
			delete exp_over;
			break;
		}
		case OP_ACCEPT: {
			std::cout << "Accept Completed.\n";
			SOCKET c_socket = *(reinterpret_cast<SOCKET*>(exp_over->_net_buf));
			int new_id = get_new_id();
			CLIENT& cl = clients[new_id];
			cl.x = 0;
			cl.y = 0;
			cl._id = new_id;
			cl._state = ST_INGAME;
			cl._prev_size = 0;
			cl._recv_over._comp_op = OP_RECV;
			cl._recv_over._wsa_buf.buf = reinterpret_cast<char*>(cl._recv_over._net_buf);
			cl._recv_over._wsa_buf.len = sizeof(cl._recv_over._net_buf);
			ZeroMemory(&cl._recv_over._wsa_over, sizeof(cl._recv_over._wsa_over));
			cl._socket = c_socket;

			CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_h_iocp, new_id, 0);
			cl.do_recv();

			ZeroMemory(&exp_over->_wsa_over, sizeof(exp_over->_wsa_over));
			c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
			*(reinterpret_cast<SOCKET*>(exp_over->_net_buf)) = c_socket;
			//64비트를 쓰기때문에 8바이트 쓰면 안된다고 말해야함.
			AcceptEx(g_s_socket, c_socket, exp_over->_net_buf + 8, 0, sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16, NULL, &exp_over->_wsa_over);
		}
		}
	}
}


int main()
{
	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(g_s_socket, SOMAXCONN);

	g_h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), g_h_iocp, 0, 0);

	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	char	accept_buf[sizeof(SOCKADDR_IN) * 2 + 32 + 100];
	EXP_OVER	accept_ex;
	*(reinterpret_cast<SOCKET*>(&accept_ex._net_buf)) = c_socket;

	ZeroMemory(&accept_ex._wsa_over, sizeof(accept_ex._wsa_over));
	accept_ex._comp_op = OP_ACCEPT;

	AcceptEx(g_s_socket, c_socket, accept_buf, 0, sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16, NULL, &accept_ex._wsa_over);
	std::cout << "Accept Called\n";
	for (int i = 0; i < MAX_USER; ++i)
		clients[i]._id = i;
	std::cout << "Creating Worker Threads\n";
	vector<thread> worker_threads;
	for (int i = 0; i < 6; ++i)
		worker_threads.emplace_back(worker);
	for (auto& th : worker_threads)
		th.join();

	//스레드가 다 끝남. 그러니까 뮤텍스 필요없음.
	for (auto& cl : clients) {
		if (ST_INGAME == cl._state)
			Disconnect(cl._id);
	}
	closesocket(g_s_socket);
	WSACleanup();
}


