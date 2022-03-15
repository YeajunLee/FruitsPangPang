// Fill out your copyright notice in the Description page of Project Settings.


#include "Network.h"
#include "MyCharacter.h"
#include "AICharacter.h"
#include "Tree.h"
#include "Punnet.h"
#include "Item.h"
#include "Projectile.h"
#include "Inventory.h"





HANDLE hiocp;

using namespace std;

Network::Network()
	: mGeneratedID(0)
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




	//s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	//ZeroMemory(&server_addr, sizeof(server_addr));
	//server_addr.sin_family = AF_INET;
	//server_addr.sin_port = htons(SERVER_PORT);
	////char server_ad[30] = "127.0.0.1";
	////cout << "������ ip�� �Է��ϼ���:";
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
	return true;
}

void Network::release()
{
	if (isInit)
	{
		mGeneratedID = 0;
		WorldCharacterCnt = 0;
		for (int i = 0; i < MAX_USER; ++i)
			mAiCharacter[i] = nullptr;
		for (int i = 0; i < MAX_USER; ++i)
			mOtherCharacter[i] = nullptr;
		/*
		WorldCharacterCnt = 0;
		mMyCharacter = nullptr;
		for (int i = 0; i < MAX_USER; ++i)
			mOtherCharacter[i] = nullptr;*/

		//closesocket(s_socket);
		//s_socket = NULL;
		WSACleanup();
		isInit = false;
	}


}

const int Network::getNewId()
{
	int Newid = mGeneratedID;
	mGeneratedID++;

	return Newid;
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

//void Network::C_Recv()
//{
//
//	DWORD recv_flag = 0;
//	ZeroMemory(&recv_expover.getWsaOver(), sizeof(recv_expover.getWsaOver()));
//
//	recv_expover.getWsaBuf().buf = reinterpret_cast<char*>(recv_expover.getBuf() + prev_size);
//	recv_expover.getWsaBuf().len = sizeof(recv_expover.getBuf()) - prev_size;
//	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, 0, &recv_flag, &recv_expover.getWsaOver(), recv_callback);
//	if (ret == SOCKET_ERROR) {
//		int err_num = WSAGetLastError();
//		if (WSA_IO_PENDING != err_num) {
//			std::cout << "err_receive" << std::endl;
//			error_display(err_num);
//		}
//	}
//}


void Network::send_login_packet(SOCKET& sock)
{
	cs_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGIN;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_login), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}


void Network::send_move_packet(SOCKET& sock, const float& x, const float& y, const float& z, FQuat& rotate, const float& value)
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
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_anim_packet(SOCKET& sock, AnimType type)
{
	cs_packet_anim packet;
	packet.size = sizeof(cs_packet_anim);
	packet.type = CS_PACKET_ANIM;
	packet.animtype = static_cast<char>(type);

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_anim), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_spawnobj_packet(SOCKET& sock, const FVector& locate, const FQuat& rotate, const FVector& scale, const int& fruitType)
{
	cs_packet_spawnobj packet;
	packet.size = sizeof(cs_packet_spawnobj);
	packet.type = CS_PACKET_SPAWNOBJ;
	packet.rx = rotate.X, packet.ry = rotate.Y, packet.rz = rotate.Z, packet.rw = rotate.W;
	packet.lx = locate.X, packet.ly = locate.Y, packet.lz = locate.Z;
	packet.sx = scale.X, packet.sy = scale.Y, packet.sz = scale.Z;
	packet.fruitType = fruitType;


	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_spawnobj), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);

}

void Network::send_getfruits_tree_packet(SOCKET& sock, const int& treeId)
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
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_getfruits_punnet_packet(SOCKET& sock, const int& punnetId)
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
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_useitem_packet(SOCKET& sock, const int& slotNum, const int& amount)
{
	cs_packet_useitem packet;
	packet.size = sizeof(cs_packet_useitem);
	packet.type = CS_PACKET_USEITEM;
	packet.slotNum = slotNum;
	packet.Amount = amount;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_hitmyself_packet(SOCKET& sock, const int& FruitType)
{
	cs_packet_hit packet;
	packet.size = sizeof(cs_packet_hit);
	packet.type = CS_PACKET_HIT;
	packet.fruitType = FruitType;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);

}

void Network::send_change_hotkeyslot_packet(SOCKET& sock, const int& slotNum)
{

	cs_packet_change_hotkeyslot packet;
	packet.size = sizeof(cs_packet_change_hotkeyslot);
	packet.type = CS_PACKET_CHANGE_HOTKEYSLOT;
	packet.HotkeySlotNum = slotNum;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::send_pos_packet(SOCKET& sock, const float& x, const float& y, const float& z, const char& type)
{
	cs_packet_pos packet;
	packet.size = sizeof(cs_packet_pos);
	packet.type = CS_PACKET_POS;
	packet.useType = POS_TYPE_DURIAN;
	packet.x = x;
	packet.y = y;
	packet.z = z;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_pos), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
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

		if (packet->canHarvest)	//���� ����
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
		else {					//��Ȯ ����

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
	//UE_LOG(LogTemp, Log, TEXT("recv_callback called"));
	if (0 > over->getId() || 8 < over->getId()) return;
	if (nullptr == Network::GetNetwork()->mAiCharacter[over->getId()]) return;
	int to_process_data = num_bytes + Network::GetNetwork()->mAiCharacter[over->getId()]->_prev_size;
	unsigned char* packet = over->getBuf();
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Network::GetNetwork()->process_packet2(static_cast<int>(over->getId()),packet);
		to_process_data -= packet_size;
		packet += packet_size;
		if (to_process_data > 0) packet_size = packet[0];
		else break;
	}
	Network::GetNetwork()->mAiCharacter[over->getId()]->_prev_size = to_process_data;
	if (to_process_data > 0)
	{
		memcpy(over->getBuf(), packet, to_process_data);
	}
	Network::GetNetwork()->mAiCharacter[over->getId()]->recvPacket();
}

void Network::process_packet2(int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	auto PacketOwner = Network::GetNetwork()->mAiCharacter[client_id];
	auto Game = Network::GetNetwork();
	switch (packet_type) {
	case SC_PACKET_LOGIN_OK: {
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(p);
		PacketOwner->c_id = packet->id;
		break;
	}
	case SC_PACKET_MOVE: {
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(p);
		int move_id = packet->id;

		//if Ai Move, dump packet
		//Ai�� �����̴� ��Ŷ�� ������. ������ Ŭ�� ������ �����̴°Ŷ� ��Ŷ���� �ȿ������� �ȴ�.
		bool escape = false;
		for (auto ai : mAiCharacter)
		{
			if (ai == nullptr) continue;
			if (move_id == ai->c_id)
			{ 
				escape = true;
				break;
			}
		}
		if (escape) break;

		UE_LOG(LogTemp, Log, TEXT("move called"));

		if (move_id < MAX_USER)
		{
			if (mOtherCharacter[move_id] != nullptr)
			{
				mOtherCharacter[move_id]->SetActorLocation(FVector(packet->x, packet->y, packet->z));
				mOtherCharacter[move_id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
				mOtherCharacter[move_id]->GroundSpeed = packet->speed;
			}
		}
		break;
	}
	case SC_PACKET_ANIM: {
		sc_packet_anim* packet = reinterpret_cast<sc_packet_anim*>(p);
		int thrower_character_id = packet->id;

		//if Ai Move, dump packet
		//Ai�� �����̴� ��Ŷ�� ������. ������ Ŭ�� ������ �����̴°Ŷ� ��Ŷ���� �ȿ������� �ȴ�.
		bool escape = false;
		for (auto ai : mAiCharacter)
		{
			if (ai == nullptr) continue;
			if (thrower_character_id == ai->c_id)
			{
				escape = true;
				break;
			}
		}
		if (escape) break;

		UE_LOG(LogTemp, Log, TEXT("Anim called"));
		switch (packet->animtype)
		{
			case static_cast<char>(Network::AnimType::Throw) :
			{
				if (thrower_character_id < MAX_USER) {
					if (mOtherCharacter[packet->id] != nullptr)
					{
						UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
						if (AnimInstance && mOtherCharacter[packet->id]->AnimThrowMontage)
						{
							AnimInstance->Montage_Play(mOtherCharacter[packet->id]->AnimThrowMontage, 2.f);
							AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->AnimThrowMontage);

						}
					}
				}
				break;
			}

		}
		break;
	}
	case SC_PACKET_PUT_OBJECT: {
		sc_packet_put_object* packet = reinterpret_cast<sc_packet_put_object*>(p);
		int id = packet->id;

		bool escape = false;
		for (auto ai : mAiCharacter)
		{
			if (ai == nullptr) continue;
			if (id == ai->c_id)
			{
				escape = true;
				break;
			}
		}
		if (escape) break;

		mOtherCharacter[id]->GetMesh()->SetVisibility(true);
		mOtherCharacter[id]->c_id = packet->id;
		mOtherCharacter[id]->s_connected = true;
		break;
	}

	case SC_PACKET_REMOVE_OBJECT: {
		sc_packet_remove_object* packet = reinterpret_cast<sc_packet_remove_object*>(p);
		int other_id = packet->id;
		break;
	}
	case SC_PACKET_SPAWNOBJ: {
		sc_packet_spawnobj* packet = reinterpret_cast<sc_packet_spawnobj*>(p);
		int other_id = packet->id;
		mOtherCharacter[other_id]->Throw(FVector(packet->lx, packet->ly, packet->lz), FRotator(packet->rx, packet->ry, packet->rz), AInventory::ItemCodeToItemBombPath(packet->fruitType));
		
		

		//FTransform SocketTransform = FTransform(FQuat(packet->rx, packet->ry, packet->rz, packet->rw), FVector(packet->lx, packet->ly, packet->lz), FVector(packet->sx, packet->sy, packet->sz));
		////FName path = TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb_Test.Bomb_Test_C'"); //_C�� �� �ٿ��� �ȴٰ� ��.
		//FName path = AInventory::ItemCodeToItemBombPath(packet->fruitType);
		//UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		//auto bomb = mOtherCharacter[other_id]->GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketTransform);
		break;
	}
	case SC_PACKET_UPDATE_INVENTORY: {
		sc_packet_update_inventory* packet = reinterpret_cast<sc_packet_update_inventory*>(p);

		FItemInfo itemClass;
		itemClass.ItemCode = packet->itemCode;
		itemClass.IndexOfHotKeySlot = packet->slotNum;
		UE_LOG(LogTemp, Error, TEXT("slot Num : %d"), packet->slotNum);
		itemClass.Name = AInventory::ItemCodeToItemName(packet->itemCode);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(packet->itemCode);
		PacketOwner->mInventory->UpdateInventorySlot(itemClass, packet->itemAmount);

		break;
	}
	case SC_PACKET_UPDATE_USERSTATUS: {
		sc_packet_update_userstatus* packet = reinterpret_cast<sc_packet_update_userstatus*>(p);
		PacketOwner->hp = packet->hp;
		//PacketOwner->mInventory->mMainWidget->UpdateHpBar();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("My HP: %d "), PacketOwner->hp));
		break;
	}
	case SC_PACKET_DIE: {
		sc_packet_die* packet = reinterpret_cast<sc_packet_die*>(p);
		//�׾����� �� �ൿ ex) ���� ui, ���� Animation, ��Ȱ ui
		//����� �׳� ������⸸ ����.
		if (packet->id == PacketOwner->c_id) {

			PacketOwner->SetActorRotation(FQuat(90, 0, 0, 1));
			//PacketOwner->DisableInput(mMyCharacter->GetWorld()->GetFirstPlayerController());

		}
		else if (packet->id < MAX_USER)
		{
			if (mOtherCharacter[packet->id] != nullptr)
			{
				if (true == mOtherCharacter[packet->id]->s_connected)
				{
					mOtherCharacter[packet->id]->SetActorRotation(FQuat(90, 0, 0, 1));
				}
			}
		}
		//

		break;
	}
	case SC_PACKET_RESPAWN: {
		sc_packet_respawn* packet = reinterpret_cast<sc_packet_respawn*>(p);

		if (packet->id == PacketOwner->c_id) {

			PacketOwner->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
			PacketOwner->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
			PacketOwner->GroundSpeed_AI = 0;
			//PacketOwner->EnableInput(PacketOwner->GetWorld()->GetFirstPlayerController());

		}
		else if (packet->id < MAX_USER)
		{
			if (mOtherCharacter[packet->id] != nullptr)
			{
				if (true == mOtherCharacter[packet->id]->s_connected)
				{
					mOtherCharacter[packet->id]->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
					mOtherCharacter[packet->id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
					mOtherCharacter[packet->id]->GroundSpeed = 0;
				}
			}
		}
		break;
	}
	case SC_PACKET_UPDATE_INTERSTAT: {
		sc_packet_update_interstat* packet = reinterpret_cast<sc_packet_update_interstat*>(p);
		//BroadCast Packet just do once
		//BroadCast Packet�� �� �ѹ��� ����ž� ��.
		if (0 != PacketOwner->overID) break;
		if (packet->canHarvest)	//���� ����
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
		else {					//��Ȯ ����

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