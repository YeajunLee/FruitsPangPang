// Fill out your copyright notice in the Description page of Project Settings.


#include "Network.h"
#include "MyCharacter.h"
#include "AICharacter.h"
#include "Tree.h"
#include "Punnet.h"




void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

HANDLE hiocp;

using namespace std;

Network::Network()
	: prev_size(0)
	, mId(0)
{
	for (int i = 0; i < 8; ++i)
	{
		mAiCharacter[i] = nullptr;
	}
	for (int i = 0; i < MAX_USER; ++i)
	{
		mOtherCharacter[i] = nullptr;
	}
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
	isInit = true;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
	//Make Thread

	//



	//s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	//ZeroMemory(&server_addr, sizeof(server_addr));
	//server_addr.sin_family = AF_INET;
	//server_addr.sin_port = htons(SERVER_PORT);
	////char server_ad[30] = "127.0.0.1";
	////cout << "접속할 ip를 입력하세요:";
	////cin.getline(server_ad, 30);
	//inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	//int rt = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	//if (SOCKET_ERROR == rt)
	//{
	//	std::cout << "connet Error :";
	//	int err_num = WSAGetLastError();
	//	error_display(err_num);
	//	cout << "connection eliminate." << endl;
	//	//system("pause");
	//	//exit(0);
	//	return false;
	//}
	//return true;
	return true;
}

void Network::release()
{
	if (isInit)
	{
		prev_size = 0;
		mId = 0;
		/*
		WorldCharacterCnt = 0;
		mMyCharacter = nullptr;
		for (int i = 0; i < MAX_USER; ++i)
			mOtherCharacter[i] = nullptr;*/

		closesocket(s_socket);
		s_socket = NULL;
		WSACleanup();
		isInit = false;
	}


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


void Network::C_Recv()
{

	DWORD recv_flag = 0;
	ZeroMemory(&recv_expover.getWsaOver(), sizeof(recv_expover.getWsaOver()));

	recv_expover.getWsaBuf().buf = reinterpret_cast<char*>(recv_expover.getBuf() + prev_size);
	recv_expover.getWsaBuf().len = sizeof(recv_expover.getBuf()) - prev_size;
	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, 0, &recv_flag, &recv_expover.getWsaOver(), recv_callback);
	if (ret == SOCKET_ERROR) {
		int err_num = WSAGetLastError();
		if (WSA_IO_PENDING != err_num) {
			std::cout << "err_receive" << std::endl;
			error_display(err_num);
		}
	}
}


void Network::send_login_packet()
{
	cs_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGIN;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_login), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}


void Network::send_move_packet(const float& x, const float& y, const float& z, FQuat& rotate, const float& value)
{
	cs_packet_move packet;
	packet.size = sizeof(cs_packet_move);
	packet.type = CS_PACKET_MOVE;
	packet.x = x;
	packet.y = y;
	packet.z = z;
	packet.rx = rotate.X;
	packet.ry = rotate.Y;
	packet.rz = rotate.Z;
	packet.rw = rotate.W;
	packet.speed = value;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_move), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_anim_packet(AnimType type)
{
	cs_packet_anim packet;
	packet.size = sizeof(cs_packet_anim);
	packet.type = CS_PACKET_ANIM;
	packet.animtype = static_cast<char>(type);

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_anim), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_spawnobj_packet(const FVector& locate, const FQuat& rotate, const FVector& scale, const int& fruitType)
{
	cs_packet_spawnobj packet;
	packet.size = sizeof(cs_packet_spawnobj);
	packet.type = CS_PACKET_SPAWNOBJ;
	packet.rx = rotate.X, packet.ry = rotate.Y, packet.rz = rotate.Z, packet.rw = rotate.W;
	packet.lx = locate.X, packet.ly = locate.Y, packet.lz = locate.Z;
	packet.sx = scale.X, packet.sy = scale.Y, packet.sz = scale.Z;
	packet.fruitType = fruitType;


	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_spawnobj), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);

}

void Network::send_getfruits_tree_packet(const int& treeId)
{
	if (treeId == -1)
	{
		//Exception Occurred
		return;
	}
	cs_packet_getfruits packet;
	packet.size = sizeof(cs_packet_getfruits);
	packet.type = CS_PACKET_GETFRUITS_TREE;
	packet.obj_id = treeId;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_getfruits_punnet_packet(const int& punnetId)
{
	if (punnetId == -1)
	{
		//Exception Occurred
		return;
	}
	cs_packet_getfruits packet;
	packet.size = sizeof(cs_packet_getfruits);
	packet.type = CS_PACKET_GETFRUITS_PUNNET;
	packet.obj_id = punnetId;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_useitem_packet(const int& slotNum, const int& amount)
{
	cs_packet_useitem packet;
	packet.size = sizeof(cs_packet_useitem);
	packet.type = CS_PACKET_USEITEM;
	packet.slotNum = slotNum;
	packet.Amount = amount;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_hitmyself_packet(const int& FruitType)
{
	cs_packet_hit packet;
	packet.size = sizeof(cs_packet_hit);
	packet.type = CS_PACKET_HIT;
	packet.fruitType = FruitType;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);

}

void Network::send_change_hotkeyslot_packet(const int& slotNum)
{

	cs_packet_change_hotkeyslot packet;
	packet.size = sizeof(cs_packet_change_hotkeyslot);
	packet.type = CS_PACKET_CHANGE_HOTKEYSLOT;
	packet.HotkeySlotNum = slotNum;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_pos_packet(const float& x, const float& y, const float& z, const char& type)
{
	cs_packet_pos packet;
	packet.size = sizeof(cs_packet_pos);
	packet.type = CS_PACKET_POS;
	packet.useType = POS_TYPE_DURIAN;
	packet.x = x;
	packet.y = y;
	packet.z = z;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_pos), &packet);
	int ret = WSASend(s_socket, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::process_packet(unsigned char* p)
{
	unsigned char Type = p[1];

	switch (Type) {
	case SC_PACKET_LOGIN_OK:{
		break;
	}
	case SC_PACKET_UPDATE_INTERSTAT: {
		sc_packet_update_interstat* packet = reinterpret_cast<sc_packet_update_interstat*>(p);

		if (packet->canHarvest)	//생성 로직
		{
			if (packet->useType == INTERACT_TYPE_TREE)
			{
				mTree[packet->objNum]->GenerateFruit(packet->fruitType);
			}
			else if (packet->useType == INTERACT_TYPE_PUNNET)
			{
				UE_LOG(LogTemp, Log, TEXT("Punnet Generate"));
				mPunnet[packet->objNum]->GenerateFruit(packet->fruitType);
			}
		}
		else {					//수확 로직

			if (packet->useType == INTERACT_TYPE_TREE)
			{
				mTree[packet->objNum]->HarvestFruit();
			}
			else if (packet->useType == INTERACT_TYPE_PUNNET)
			{
				UE_LOG(LogTemp, Log, TEXT("Punnet Harvest"));
				mPunnet[packet->objNum]->HarvestFruit();
			}

		}
		break;
	}
	default: {
		//Unknwon Packet Error
		break; 
	}
	}
}















void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{
	//cout << "send_callback is called" << endl;
	WSA_OVER_EX* once_exp = reinterpret_cast<WSA_OVER_EX*>(send_over);
	delete once_exp;
}

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	WSA_OVER_EX* over = reinterpret_cast<WSA_OVER_EX*>(recv_over);
	int to_process_data = num_bytes + Network::GetNetwork()->prev_size;
	unsigned char* packet = over->getBuf();
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
		memcpy(over->getBuf(), packet, to_process_data);
	}
	Network::GetNetwork()->C_Recv();
}



void Worker_Thread()
{
	while (true) {
		DWORD bytes;
		LONG64 iocp_key;
		WSAOVERLAPPED* overlapped;

		BOOL ret = GetQueuedCompletionStatus(hiocp, &bytes, (PULONG_PTR)&iocp_key, &overlapped, INFINITE);
		int client_id = static_cast<int>(iocp_key);
		WSA_OVER_EX* wsa_ex = reinterpret_cast<WSA_OVER_EX*>(overlapped);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			std::cout << "GQCS Error";
			//error_display(err_no);
			//Disconnect(client_id);
			if (wsa_ex->getCmd() == CMD_SEND)
				delete wsa_ex;
			continue;
		}


		switch (wsa_ex->getCmd()) {
		case CMD_RECV: {
			if (bytes == 0) {
				//Disconnect(client_id);
				break;
			}

			auto player = Network::GetNetwork()->mAiCharacter[client_id];			
			//Character* player = reinterpret_cast<Character*>(objects[client_id]);
			int To_Process_Bytes = bytes + player->_prev_size;
			unsigned char* packets = wsa_ex->getBuf();

			while (To_Process_Bytes >= packets[0]) {
				process_packet2(client_id, packets);
				To_Process_Bytes -= packets[0];
				packets += packets[0];
				if (To_Process_Bytes <= 0)break;
			}
			player->_prev_size = 0;
			ZeroMemory(wsa_ex->getBuf(), sizeof(wsa_ex->getBuf()));

			if (To_Process_Bytes > 0) {

				player->_prev_size = To_Process_Bytes;
				memcpy(wsa_ex->getBuf(), packets, player->_prev_size);
			}
			player->recvPacket();
			break;
		}

		case CMD_SEND: {
			//if (num_byte != wsa_ex->_wsa_buf.len) {
			//	Disconnect(client_id);
			//}
			delete wsa_ex;
			break;
		}
	}
	}
}


void process_packet2(int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	auto PacketOwner = Network::GetNetwork()->mAiCharacter[client_id];
	auto Game = Network::GetNetwork();
	switch (packet_type) {
	case SC_PACKET_LOGIN_OK: {
		break;
	}
	case SC_PACKET_UPDATE_INTERSTAT: {
		sc_packet_update_interstat* packet = reinterpret_cast<sc_packet_update_interstat*>(p);

		if (packet->canHarvest)	//생성 로직
		{
			if (packet->useType == INTERACT_TYPE_TREE)
			{
				Game->mTree[packet->objNum]->GenerateFruit(packet->fruitType);
			}
			else if (packet->useType == INTERACT_TYPE_PUNNET)
			{
				UE_LOG(LogTemp, Log, TEXT("Punnet Generate"));
				Game->mPunnet[packet->objNum]->GenerateFruit(packet->fruitType);
			}
		}
		else {					//수확 로직

			if (packet->useType == INTERACT_TYPE_TREE)
			{
				Game->mTree[packet->objNum]->HarvestFruit();
			}
			else if (packet->useType == INTERACT_TYPE_PUNNET)
			{
				UE_LOG(LogTemp, Log, TEXT("Punnet Harvest"));
				Game->mPunnet[packet->objNum]->HarvestFruit();
			}

		}
		break;
	}
	default: {
		//Unknwon Packet Error
		break;
	}
	}
}