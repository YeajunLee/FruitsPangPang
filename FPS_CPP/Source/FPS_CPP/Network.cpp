// Fill out your copyright notice in the Description page of Project Settings.


#include "Network.h"
#include "MyCharacter.h"

//#ifdef _DEBUG
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

using namespace std;

Network::Network() 
	: prev_size(0)
	, mMyCharacter(nullptr)
{
}

Network::~Network()
{
	closesocket(s_socket);
	WSACleanup();
}

std::shared_ptr<Network> Network::GetNetwork()
{
	if (m_Network.use_count() == 0)
	{
		m_Network = std::make_shared<Network>();
		return m_Network;
	}
	return m_Network;
}

bool Network::init()
{
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	//char server_ad[30] = "127.0.0.1";
	//cout << "접속할 ip를 입력하세요:";
	//cin.getline(server_ad, 30);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	int rt = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		error_display(err_num);
		cout << "connection eliminate." << endl;
		//system("pause");
		//exit(0);
		return true;
	}
	return false;
}


void Network::error_display(int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, 0);
	std::wcout << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}
void Network::C_Send()
{
	//

	//
	DWORD sent_byte = 0;
	WSABUF mybuf;
	mybuf.buf = reinterpret_cast<char*>(buf);
	mybuf.len = BUFSIZE;

	WSAOVERLAPPED* send_over = new WSAOVERLAPPED;
	ZeroMemory(send_over, sizeof(*send_over));

	int ret = WSASend(s_socket, &mybuf, 1, 0, 0, send_over, send_callback);
	//SleepEx(10, true);
	//int err_num = WSAGetLastError();
	//error_display(err_num);
}

void Network::C_Recv()
{
	std::cout << "here's commin?" << std::endl;

	DWORD recv_flag = 0;
	ZeroMemory(&recv_expover._wsa_over, sizeof(recv_expover._wsa_over));
	
	recv_expover._wsa_buf.buf = reinterpret_cast<char*>(recv_expover._my_buf + prev_size);
	recv_expover._wsa_buf.len = sizeof(recv_expover._my_buf) - prev_size;
	int ret = WSARecv(s_socket, &recv_expover._wsa_buf, 1, 0, &recv_flag, &recv_expover._wsa_over, recv_callback);
	if (ret == SOCKET_ERROR) {
		int err_num = WSAGetLastError();
		if (WSA_IO_PENDING != err_num) {
			std::cout << "err_receive" << std::endl;
			error_display(err_num);
		}
	}
}

void Network::send_dir_packet(const float& x,const float& y,const float& z)
{
	cs_packet_dir packet;
	packet.size = sizeof(cs_packet_dir);
	packet.type = CS_PACKET_DIR;
	packet.x = x;
	packet.y = y;
	packet.z = y;

	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_dir), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);
}

void Network::process_packet(unsigned char* p)
{
	unsigned char Type = p[1];
	switch (Type) {
	case SC_PACKET_LOGIN_OK: {
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(p);
		break;
	}
	case SC_PACKET_DIR: {
		sc_packet_dir* packet = reinterpret_cast<sc_packet_dir*>(p);
		if (!packet->isValid)
		{
			if(mMyCharacter!=NULL)
				mMyCharacter->SetActorLocation(FVector(packet->x, packet->y, packet->z));
		}
		break;
	}
	}
}

void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{
	cout << "send_callback is called" << endl;
	EXP_OVER* once_exp = reinterpret_cast<EXP_OVER*>(send_over);
	delete once_exp;
	//n_Game->getNetwork()->C_Send();
	//do_send();
}
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	EXP_OVER* over = reinterpret_cast<EXP_OVER*>(recv_over);
	int to_process_data = num_bytes + Network::GetNetwork()->prev_size;
	unsigned char* packet = over->_my_buf;
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Network::GetNetwork()->process_packet(packet);
		to_process_data -= packet_size;
		packet += packet_size;
		if (to_process_data > 0) packet_size = packet[0];
		else break;
	}
	Network::GetNetwork()->prev_size = to_process_data;
	if (to_process_data > 0)
	{
		memcpy(&over->_my_buf, packet, to_process_data);
	}
	Network::GetNetwork()->C_Recv();
}
