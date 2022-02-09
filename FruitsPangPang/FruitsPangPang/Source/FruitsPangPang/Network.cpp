// Fill out your copyright notice in the Description page of Project Settings.


#include "Network.h"
#include "MyCharacter.h"
#include "Tree.h"
#include "Inventory.h"
#include "Item.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainWidget.h"

//#ifdef _DEBUG
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

using namespace std;

Network::Network()
	: prev_size(0)
	, mMyCharacter(nullptr)
	, WorldCharacterCnt(0)
	, mId(0)
{
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
		return false;
	}
	return true;
}

void Network::release()
{
	if (isInit)
	{
		prev_size = 0;
		WorldCharacterCnt = 0;
		mId = 0;
		mMyCharacter = nullptr;
		for (int i = 0; i < MAX_USER; ++i)
			mOtherCharacter[i] = nullptr;

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

void Network::send_login_packet()
{
	cs_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGIN;

	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_login), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);
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
	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_move), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);
}

void Network::send_anim_packet(AnimType type)
{
	cs_packet_anim packet;
	packet.size = sizeof(cs_packet_anim);
	packet.type = CS_PACKET_ANIM;
	packet.animtype = static_cast<char>(type);

	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_anim), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);
}

void Network::send_spawnobj_packet(const FVector& locate, const FQuat& rotate, const FVector& scale)
{
	cs_packet_spawnobj packet;
	packet.size = sizeof(cs_packet_spawnobj);
	packet.type = CS_PACKET_SPAWNOBJ;
	packet.rx = rotate.X, packet.ry = rotate.Y, packet.rz = rotate.Z, packet.rw = rotate.W;
	packet.lx = locate.X, packet.ly = locate.Y, packet.lz = locate.Z;
	packet.sx = scale.X, packet.sy = scale.Y, packet.sz = scale.Z;


	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_spawnobj), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);

}

void Network::send_getfruits_packet(const int& treeId)
{
	if (treeId == -1)
	{
		//Exception Occurred
		return;
	}
	cs_packet_getfruits packet;
	packet.size = sizeof(cs_packet_getfruits);
	packet.type = CS_PACKET_GETFRUITS;
	packet.tree_id = treeId;

	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);
}

void Network::send_useitem_packet(const int& slotNum, const int& amount)
{
	cs_packet_useitem packet;
	packet.size = sizeof(cs_packet_useitem);
	packet.type = CS_PACKET_USEITEM;
	packet.slotNum = slotNum;
	packet.Amount = amount;

	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);
}

void Network::send_hitmyself_packet(const int& FruitType)
{
	cs_packet_hit packet;
	packet.size = sizeof(cs_packet_hit);
	packet.type = CS_PACKET_HIT;
	packet.fruitType = FruitType;

	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);
	
}

void Network::send_change_hotkeyslot_packet(const int& slotNum)
{

	cs_packet_change_hotkeyslot packet;
	packet.size = sizeof(cs_packet_change_hotkeyslot);
	packet.type = CS_PACKET_CHANGE_HOTKEYSLOT;
	packet.HotkeySlotNum = slotNum;

	EXP_OVER* once_exp = new EXP_OVER(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(s_socket, &once_exp->_wsa_buf, 1, 0, 0, &once_exp->_wsa_over, send_callback);
}

void Network::process_packet(unsigned char* p)
{
	unsigned char Type = p[1];
	switch (Type) {
	case SC_PACKET_LOGIN_OK: {
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(p);
		mMyCharacter->c_id = packet->id;
		mId = packet->id;
		mMyCharacter->mInventory->ClearInventory();
		break;
	}
	case SC_PACKET_MOVE: {
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(p);
		int move_id = packet->id;
		if (move_id == mId) {

			//if (mMyCharacter != nullptr)
			//	mMyCharacter->SetActorLocation(FVector(packet->x, packet->y, packet->z));

		}
		else if (move_id < MAX_USER)
		{
			if (mOtherCharacter[move_id] != nullptr)
			{
				mOtherCharacter[move_id]->SetActorLocation(FVector(packet->x, packet->y, packet->z));
				mOtherCharacter[move_id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
				mOtherCharacter[move_id]->GroundSpeedd = packet->speed;
			}
		}
		break;
	}
	case SC_PACKET_ANIM: {
		sc_packet_anim* packet = reinterpret_cast<sc_packet_anim*>(p);
		int thrower_character_id = packet->id;

		switch (packet->animtype)
		{
		case static_cast<char>(Network::AnimType::Throw): {

			if (thrower_character_id == mId) {

				//if (mMyCharacter != nullptr)
				//	mMyCharacter->SetActorLocation(FVector(packet->x, packet->y, packet->z));

			}
			else if (thrower_character_id < MAX_USER) {
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
		mOtherCharacter[id]->GetMesh()->SetVisibility(true);
		mOtherCharacter[id]->c_id = packet->id;
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

		FTransform SocketTransform = FTransform(FQuat(packet->rx, packet->ry, packet->rz, packet->rw), FVector(packet->lx, packet->ly, packet->lz), FVector(packet->sx, packet->sy, packet->sz));
		FName path = TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb_Test.Bomb_Test_C'"); //_C를 꼭 붙여야 된다고 함.
		UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		auto bomb = mOtherCharacter[other_id]->GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketTransform);
		break;
	}
	case SC_PACKET_UPDATE_INVENTORY: {
		sc_packet_update_inventory* packet = reinterpret_cast<sc_packet_update_inventory*>(p);

		FItemInfo itemClass;
		itemClass.ItemCode = packet->itemCode;
		itemClass.IndexOfHotKeySlot = packet->slotNum;
		itemClass.Name = mMyCharacter->mInventory->ItemCodeToItemName(packet->itemCode);
		itemClass.Icon = mMyCharacter->mInventory->ItemCodeToItemIcon(packet->itemCode);
		mMyCharacter->mInventory->UpdateInventorySlot(itemClass, packet->itemAmount);

		break;
	}
	case SC_PACKET_UPDATE_TREESTAT: {
		sc_packet_update_treestat* packet = reinterpret_cast<sc_packet_update_treestat*>(p);
		if (packet->canHarvest)	//생성 로직
		{
			mTree[packet->treeNum]->GenerateFruit(packet->fruitType);
		}
		else {					//수확 로직
			mTree[packet->treeNum]->HarvestFruit();

		}
		break;
	}
	case SC_PACKET_UPDATE_USERSTATUS: {
		sc_packet_update_userstatus* packet = reinterpret_cast<sc_packet_update_userstatus*>(p);
		mMyCharacter->hp = packet->hp;
		mMyCharacter->mInventory->mMainWidget->UpdateHpBar();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("My HP: %d "), mMyCharacter->hp));
		break;
	}
	case SC_PACKET_DIE: {
		sc_packet_die* packet = reinterpret_cast<sc_packet_die*>(p);
		//죽었을때 할 행동 ex) 죽은 ui, 죽은 Animation, 부활 ui
		//현재는 그냥 꺾어놓기만 했음.
		if (packet->id == mId) {

			mMyCharacter->SetActorRotation(FQuat(90, 0, 0, 1));
			mMyCharacter->DisableInput(mMyCharacter->GetWorld()->GetFirstPlayerController());

		}
		else if (packet->id < MAX_USER)
		{
			if (mOtherCharacter[packet->id] != nullptr)
			{
				mOtherCharacter[packet->id]->SetActorRotation(FQuat(90, 0, 0, 1));
			}
		}
		//

		break;
	}
	case SC_PACKET_RESPAWN: {
		sc_packet_respawn* packet = reinterpret_cast<sc_packet_respawn*>(p);

		if (packet->id == mId) {
			
			mMyCharacter->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
			mMyCharacter->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
			mMyCharacter->GroundSpeedd = 0;
			mMyCharacter->EnableInput(mMyCharacter->GetWorld()->GetFirstPlayerController());
			
		}
		else if (packet->id < MAX_USER)
		{
			if (mOtherCharacter[packet->id] != nullptr)
			{
				mOtherCharacter[packet->id]->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
				mOtherCharacter[packet->id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
				mOtherCharacter[packet->id]->GroundSpeedd = 0;
			}
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
