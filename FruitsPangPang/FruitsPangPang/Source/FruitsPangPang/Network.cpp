// Fill out your copyright notice in the Description page of Project Settings.


#include "Network.h"
#include "MyCharacter.h"
#include "AICharacter.h"
#include "Tree.h"
#include "Punnet.h"
#include "HealSpawner.h"
#include "Inventory.h"
#include "Item.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainWidget.h"
#include "ScoreWidget.h"
#include "GameResultWidget.h"
#include "GameMatchWidget.h"
#include "PointOfInterestWidget.h"
#include "GoldIconWidget.h"
#include "SilverIconWidget.h"
#include "BronzeIconWidget.h"
#include "MiniMapWidget.h"
#include "MessageBoxWidget.h"
#include "AIController_Custom.h"
#include "AI_Sword_Controller_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "PointOfInterestComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StoreWidget.h"

//#ifdef _DEBUG
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_Gamecallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
void CALLBACK recv_Lobbycallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
void CALLBACK recv_Aicallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

using namespace std;

Network::Network()
	: mMyCharacter(nullptr)
	, mGeneratedID(0)
	,isInit(false)
	,mSyncBananaID(0)
{
	for (auto& p : mTree)
		p = nullptr;
	for (auto& p : mPunnet)
		p = nullptr;
	for (auto& p : mHealSpawner)
		p = nullptr;
	for (auto& p : mAiCharacter)
		p = nullptr;
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
	if (!isInit)
	{
		isInit = true; 
		WSAStartup(MAKEWORD(2, 2), &WSAData);
		return true;
	}
	return false;
}

void Network::release()
{
	if (isInit)
	{
		mGeneratedID = 0;
		mMyCharacter = nullptr;
		for (auto& p : mAiCharacter)
			p = nullptr;
		for (auto& p : mOtherCharacter)
			p = nullptr;
		mSyncBananaID = 0;
		WSACleanup();
		if (!bLevelOpenTriggerEnabled)	//openlevel로 인한 release가 아니라, editor중지때문에 생기는 release라면 false시켜줌.
		{
			bLoginFlag = false;
			bLevelOpenTriggerEnabled = false;	//editor중지때문이니까 여기도 그냥 false로 다시 초기화.
		}
		isInit = false;
	}
	

}

const int Network::getNewId()
{
	int Newid = mGeneratedID;
	mGeneratedID++;

	return Newid;
}

const int Network::getNewBananaId()
{
	int Newid = mSyncBananaID;
	mSyncBananaID++;

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

void send_login_packet(SOCKET& sock,const char& type)
{
	cs_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGIN;
	packet.cType = type;
	strcpy_s(packet.name, TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterName));
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_login_lobby_packet(SOCKET& sock, const char* name, const char* password)
{
	cl_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CL_PACKET_LOGIN;
	strcpy_s(packet.name, name);
	strcpy_s(packet.password, password);
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}


void send_signup_packet(SOCKET& sock, const char* name, const char* password)
{
	cl_packet_signup packet;
	packet.size = sizeof(packet);
	packet.type = CL_PACKET_SIGNUP;
	strcpy_s(packet.name, name);
	strcpy_s(packet.password, password);
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_move_packet(SOCKET& sock, const float& x, const float& y, const float& z, FQuat& rotate, const float& value)
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

void send_anim_packet(SOCKET& sock, Network::AnimType type)
{
	cs_packet_anim packet;
	packet.size = sizeof(cs_packet_anim);
	packet.type = CS_PACKET_ANIM;
	packet.animtype = static_cast<char>(type);

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_anim), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_spawnitemobj_packet(SOCKET& sock, const FVector& locate, const FRotator& rotate, const FVector& scale,
	const int& fruitType, const int& itemSlotNum, const int& uniqueid)
{
	cs_packet_spawnitemobj packet;
	packet.size = sizeof(cs_packet_spawnitemobj);
	packet.type = CS_PACKET_SPAWNITEMOBJ;
	packet.rx = rotate.Pitch, packet.ry = rotate.Yaw, packet.rz = rotate.Roll, packet.rw = 0.0f;
	packet.lx = locate.X, packet.ly = locate.Y, packet.lz = locate.Z;
	packet.sx = scale.X, packet.sy = scale.Y, packet.sz = scale.Z;
	packet.fruitType = fruitType;
	packet.itemSlotNum = itemSlotNum;
	packet.uniquebananaid = uniqueid;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);

}

void send_getfruits_tree_packet(SOCKET& sock, const int& treeId)
{
	if (treeId == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("UnExpected Tree ID"));
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

void send_getfruits_punnet_packet(SOCKET& sock, const int& punnetId)
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

void send_getfruits_healspawner_packet(SOCKET& sock, const int& healspawnerId)
{
	if (healspawnerId == -1)
	{
		//Exception Occurred
		return;
	}
	cs_packet_getfruits packet;
	packet.size = sizeof(cs_packet_getfruits);
	packet.type = CS_PACKET_GETFRUITS_HEAL;
	packet.obj_id = healspawnerId;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_getfruits), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_useitem_packet(SOCKET& sock, const int& slotNum, const int& amount)
{
	cs_packet_useitem packet;
	packet.size = sizeof(cs_packet_useitem);
	packet.type = CS_PACKET_USEITEM;
	packet.slotNum = slotNum;
	packet.Amount = amount;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_useitem), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_hitmyself_packet(SOCKET& sock, const int& AttackerId, const int& FruitType)
{
	cs_packet_hit packet;
	packet.size = sizeof(cs_packet_hit);
	packet.type = CS_PACKET_HIT;
	packet.fruitType = FruitType;
	packet.attacker_id = AttackerId;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_hit), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);

}

void send_change_hotkeyslot_packet(SOCKET& sock, const int& slotNum)
{

	cs_packet_change_hotkeyslot packet;
	packet.size = sizeof(cs_packet_change_hotkeyslot);
	packet.type = CS_PACKET_CHANGE_HOTKEYSLOT;
	packet.HotkeySlotNum = slotNum;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_change_hotkeyslot), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_pos_packet(SOCKET& sock, const float& x, const float& y, const float& z, const char& type)
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

void send_respawn_packet(SOCKET& sock,const char& WannaRespawn)
{
	cs_packet_select_respawn packet;
	packet.size = sizeof(cs_packet_select_respawn);
	packet.type = CS_PACKET_SELECT_RESPAWN;
	packet.numbering = WannaRespawn;


	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(cs_packet_select_respawn), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_PreGameSettingComplete_packet(SOCKET& sock)
{
	cs_packet_pregamesettingcomplete packet;
	packet.size = sizeof(cs_packet_pregamesettingcomplete);
	packet.type = CS_PACKET_PREGAMESETTINGCOMPLETE;


	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}


void send_Cheat(SOCKET& sock, const char& cheatNum, const char& FruitType)
{
	cs_packet_cheat packet;
	packet.size = sizeof(cs_packet_cheat);
	packet.type = CS_PACKET_CHEAT;
	packet.cheatType = cheatNum;
	packet.itemType = FruitType;

	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_sync_banana(SOCKET& sock, const FVector& locate, const FRotator& rotate, const int& bananaid)
{
	cs_packet_sync_banana packet;
	packet.size = sizeof(cs_packet_sync_banana);
	packet.type = CS_PACKET_SYNC_BANANA;
	packet.bananaid = bananaid;
	packet.rx = rotate.Pitch, packet.ry = rotate.Yaw, packet.rz = rotate.Roll, packet.rw = 0.0f;
	packet.lx = locate.X, packet.ly = locate.Y, packet.lz = locate.Z;


	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_match_request(SOCKET& sock, const short& Amount)
{
	cl_packet_match_request packet;
	packet.size = sizeof(cl_packet_match_request);
	packet.type = CL_PACKET_MATCH_REQUEST;
	packet.amount = Amount;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_buy_packet(SOCKET& sock, const int& itemcode)
{
	cl_packet_buy packet;
	packet.size = sizeof(cl_packet_buy);
	packet.type = CL_PACKET_BUY;
	packet.itemcode = itemcode;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void send_equip_packet(SOCKET& sock, const int& itemcode)
{
	cl_packet_equip packet;
	packet.size = sizeof(packet);
	packet.type = CL_PACKET_EQUIP;
	packet.itemcode = itemcode;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->getWsaBuf(), 1, 0, 0, &once_exp->getWsaOver(), send_callback);
}

void Network::process_packet(unsigned char* p)
{
	unsigned char Type = p[1];
	switch (Type) {
	case SC_PACKET_LOGIN_OK: {
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(p);
		mMyCharacter->c_id = packet->id;
		mMyCharacter->skinType = packet->skintype;
		mMyCharacter->EquipSkin();
		mMyCharacter->CharacterName = FString(ANSI_TO_TCHAR(packet->name));
		//mId = packet->id;
		for (int i = 0; i < TREE_CNT; ++i)
		{
			if (nullptr != mTree[i])
			{
				mTree[i]->GenerateFruit(packet->TreeFruits[i]);
				//mTree[i]->POIcomp->InitializeComponent();
			}
		}
		for (int i = 0; i < PUNNET_CNT; ++i)
		{
			if (nullptr != mPunnet[i])
			{
				mPunnet[i]->GenerateFruit(packet->PunnetFruits[i]);
			}
		}
		for (int i = 0; i < HEAL_CNT; ++i)
		{
			if (nullptr != mHealSpawner[i])
			{
				mHealSpawner[i]->GenerateFruit(packet->HealFruits[i]);
			}
		}
		mMyCharacter->mInventory->ClearInventory();
		
		break;
	}
	case SC_PACKET_MOVE: {
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(p);
		int move_id = packet->id;
		if (USER_START<= move_id && move_id < MAX_USER)
		{
			if (move_id == mMyCharacter->c_id)
			{
				//내가 움직인건 처리하지 않는다.
			}
			else if (mOtherCharacter[move_id] != nullptr)
			{
				mOtherCharacter[move_id]->SetActorLocation(FVector(packet->x, packet->y, packet->z));
				mOtherCharacter[move_id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
				mOtherCharacter[move_id]->ServerStoreGroundSpeed = packet->speed;
				mOtherCharacter[move_id]->GroundSpeedd = packet->speed;
			}
		}else{
			UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_MOVE id: %d"), move_id);
		}
		break;
	}
	case SC_PACKET_ANIM: {
		sc_packet_anim* packet = reinterpret_cast<sc_packet_anim*>(p);
		int anim_character_id = packet->id;

		switch (packet->animtype)
		{
		case static_cast<char>(Network::AnimType::Throw): {

			if (USER_START<= anim_character_id && anim_character_id < MAX_USER) {
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
			else {
				UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
			}
			break;
		}
		case static_cast<char>(Network::AnimType::PickSword_GreenOnion) : {
			if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
				if (mOtherCharacter[packet->id] != nullptr)
				{
					UE_LOG(LogTemp, Log, TEXT("Pick Sword Anima"));
					UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
					if (AnimInstance && mOtherCharacter[packet->id]->PickSwordMontage)
					{
						AnimInstance->Montage_Play(mOtherCharacter[packet->id]->PickSwordMontage, 2.f);
						AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->PickSwordMontage);
						mOtherCharacter[packet->id]->SM_GreenOnion->SetHiddenInGame(false, false);
						mOtherCharacter[packet->id]->SM_Carrot->SetHiddenInGame(true, false);

					}
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
			}
			break;
		}
		case static_cast<char>(Network::AnimType::PickSword_Carrot) : {
			if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
				if (mOtherCharacter[packet->id] != nullptr)
				{
					UE_LOG(LogTemp, Log, TEXT("Pick Sword Anima"));
					UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
					if (AnimInstance && mOtherCharacter[packet->id]->PickSwordMontage)
					{
						AnimInstance->Montage_Play(mOtherCharacter[packet->id]->PickSwordMontage, 2.f);
						AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->PickSwordMontage);
						mOtherCharacter[packet->id]->SM_GreenOnion->SetHiddenInGame(true, false);
						mOtherCharacter[packet->id]->SM_Carrot->SetHiddenInGame(false, false);

					}
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
			}
			break;
		}
		case static_cast<char>(Network::AnimType::DropSword) : {
			if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
				if (mOtherCharacter[packet->id] != nullptr)
				{
					mOtherCharacter[packet->id]->SM_GreenOnion->SetHiddenInGame(true, false);
					mOtherCharacter[packet->id]->SM_Carrot->SetHiddenInGame(true, false);
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
			}
			break;
		}
		case static_cast<char>(Network::AnimType::Slash) : {
			if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
				if (mOtherCharacter[packet->id] != nullptr)
				{
					UE_LOG(LogTemp, Log, TEXT("Slash Anima"));
					UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
					if (AnimInstance && mOtherCharacter[packet->id]->SlashMontage)
					{
						AnimInstance->Montage_Play(mOtherCharacter[packet->id]->SlashMontage, 1.5f);
						AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->SlashMontage);

					}
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
			}
			break;
		}
		case static_cast<char>(Network::AnimType::Stab) : {
			if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
				if (mOtherCharacter[packet->id] != nullptr)
				{
					UE_LOG(LogTemp, Log, TEXT("Slash Anima"));
					UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
					if (AnimInstance && mOtherCharacter[packet->id]->StabbingMontage)
					{
						AnimInstance->Montage_Play(mOtherCharacter[packet->id]->StabbingMontage, 1.2f);
						AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->StabbingMontage);

					}
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
			}
			break;
		}

		}
		break;
	}
	case SC_PACKET_PUT_OBJECT: {
		sc_packet_put_object* packet = reinterpret_cast<sc_packet_put_object*>(p);
		int id = packet->id;
		if (nullptr != mOtherCharacter[id])
		{
			mOtherCharacter[id]->GetMesh()->SetVisibility(true);
			mOtherCharacter[id]->c_id = packet->id;
			mOtherCharacter[id]->CharacterName = FString(ANSI_TO_TCHAR(packet->name));
			mOtherCharacter[id]->skinType = packet->skintype;
			mOtherCharacter[id]->EquipSkin();
			mMyCharacter->mInventory->mMainWidget->mScoreWidget->ScoreBoard.push_back(ScoreInfo(mOtherCharacter[id]));
			mMyCharacter->mInventory->mMainWidget->mScoreWidget->UpdateRank();
			
		}
		else {
			FName path = TEXT("Blueprint'/Game/Character/BP_MyCharacter.BP_MyCharacter_C'"); //_C를 꼭 붙여야 된다고 함.
			UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
			FTransform trans(FQuat(packet->rx, packet->ry, packet->rz, packet->rw), FVector(10020, 12760, (id + 1) * 140));
			auto mc = mMyCharacter->GetWorld()->SpawnActorDeferred<AMyCharacter>(GeneratedInventoryBP, trans);
			if (nullptr != mc)
			{
				mc->SpawnDefaultController();
				mc->AutoPossessPlayer = EAutoReceiveInput::Disabled;
				mc->FinishSpawning(trans);
				mOtherCharacter[id] = mc;
				mOtherCharacter[id]->GetMesh()->SetVisibility(true);
				mOtherCharacter[id]->c_id = packet->id;
				mOtherCharacter[id]->CharacterName = FString(ANSI_TO_TCHAR(packet->name));
				mOtherCharacter[id]->skinType = packet->skintype;
				mOtherCharacter[id]->EquipSkin();
				mMyCharacter->mInventory->mMainWidget->mScoreWidget->ScoreBoard.push_back(ScoreInfo(mOtherCharacter[id]));
				mMyCharacter->mInventory->mMainWidget->mScoreWidget->UpdateRank();
				
			}
		}

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

		//FTransform SocketTransform = FTransform(FQuat(packet->rx, packet->ry, packet->rz, packet->rw), FVector(packet->lx, packet->ly, packet->lz), FVector(packet->sx, packet->sy, packet->sz));
		////FName path = TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb_Test.Bomb_Test_C'"); //_C를 꼭 붙여야 된다고 함.
		//FName path = AInventory::ItemCodeToItemBombPath(packet->fruitType);
		//UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));

		mOtherCharacter[other_id]->Throw(FVector(packet->lx, packet->ly, packet->lz), FRotator(packet->rx, packet->ry, packet->rz),packet->fruitType,packet->uniqueid);

		//auto bomb = mOtherCharacter[other_id]->GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketTransform);
		break;
	}
	case SC_PACKET_UPDATE_INVENTORY: {
		sc_packet_update_inventory* packet = reinterpret_cast<sc_packet_update_inventory*>(p);

		FItemInfo itemClass;
		itemClass.ItemCode = packet->itemCode;
		itemClass.IndexOfHotKeySlot = packet->slotNum;
		itemClass.Name = AInventory::ItemCodeToItemName(packet->itemCode);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(packet->itemCode);
		auto character = Cast<AMyCharacter>(mMyCharacter);
		if (nullptr != character)
		{
			character->mInventory->UpdateInventorySlot(itemClass, packet->itemAmount);

			if ((7 == itemClass.ItemCode || 8 == itemClass.ItemCode) && (2 == character->SelectedHotKeySlotNum))
			{

				character->PickSwordAnimation();
					//검을 들고있다면
					
			}

		}

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
				//UE_LOG(LogTemp, Log, TEXT("Punnet Generate"));
				mPunnet[packet->objNum]->GenerateFruit(packet->fruitType);
			}
			else if (packet->useType == INTERACT_TYPE_HEAL)
			{
				//UE_LOG(LogTemp, Log, TEXT("Heal Harvest"));
				mHealSpawner[packet->objNum]->GenerateFruit(packet->fruitType);
			}
		}
		else {					//수확 로직

			if (packet->useType == INTERACT_TYPE_TREE)
			{
				mTree[packet->objNum]->HarvestFruit();
			}
			else if (packet->useType == INTERACT_TYPE_PUNNET)
			{
				//UE_LOG(LogTemp, Log, TEXT("Punnet Harvest"));
				mPunnet[packet->objNum]->HarvestFruit();
			}
			else if (packet->useType == INTERACT_TYPE_HEAL)
			{
				//UE_LOG(LogTemp, Log, TEXT("Heal Harvest"));
				mHealSpawner[packet->objNum]->HarvestFruit();
			}

		}
		break;
	}
	case SC_PACKET_UPDATE_USERSTATUS: {
		sc_packet_update_userstatus* packet = reinterpret_cast<sc_packet_update_userstatus*>(p);
		mMyCharacter->hp = packet->hp;
		mMyCharacter->mInventory->mMainWidget->UpdateHpBar();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		//	FString::Printf(TEXT("My HP: %d "), mMyCharacter->hp));
		break;
	}
	case SC_PACKET_DIE: {
		sc_packet_die* packet = reinterpret_cast<sc_packet_die*>(p);
		//죽었을때 할 행동 ex) 죽은 ui, 죽은 Animation, 부활 ui
		//현재는 그냥 꺾어놓기만 했음.
		if (packet->id == mMyCharacter->c_id) {

			mMyCharacter->DisableInput(mMyCharacter->GetWorld()->GetFirstPlayerController());
			mMyCharacter->bIsDie = true;
			mMyCharacter->bAttacking = false;
			mMyCharacter->bLMBDown = false;
			mMyCharacter->SetActorEnableCollision(false);

			mMyCharacter->mInventory->mMainWidget->ShowRespawnWidget();
			//UE_LOG(LogTemp, Log, TEXT("Die Packet received"));

		}
		else if (packet->id < MAX_USER)
		{
			if (mOtherCharacter[packet->id] != nullptr)
			{
				mOtherCharacter[packet->id]->bIsDie = true;
				mOtherCharacter[packet->id]->SetActorEnableCollision(false);
				
			}
		}
		//

		break;
	}
	case SC_PACKET_RESPAWN: {
		sc_packet_respawn* packet = reinterpret_cast<sc_packet_respawn*>(p);

		if (packet->id == mMyCharacter->c_id) {
			
			mMyCharacter->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
			mMyCharacter->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
			mMyCharacter->GroundSpeedd = 0;
			mMyCharacter->EnableInput(mMyCharacter->GetWorld()->GetFirstPlayerController());
			mMyCharacter->mInventory->mMainWidget->HideRespawnWidget();

			mMyCharacter->bIsDie = false;
			mMyCharacter->bAttacking = false;
			mMyCharacter->SetActorEnableCollision(true);
			
		}
		else if (packet->id < MAX_USER)
		{
			if (mOtherCharacter[packet->id] != nullptr)
			{
				mOtherCharacter[packet->id]->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
				mOtherCharacter[packet->id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
				mOtherCharacter[packet->id]->GroundSpeedd = 0;

				mOtherCharacter[packet->id]->bIsDie = false;
				mOtherCharacter[packet->id]->SetActorEnableCollision(true);
			}
		}
		break;
	}
	case SC_PACKET_UPDATE_SCORE: {
		sc_packet_update_score* packet = reinterpret_cast<sc_packet_update_score*>(p);
		for (int i = USER_START; i < MAX_USER; ++i)
		{
			//이거 고쳐야함 버그 있음.
			//서버에서 받아오는게 무조건 c_id 0번째부터 character id 0번째부터 killcount 0번째에 넣어주는게 아님. 지금은 그런데 나중엔 어떻게될지모름.
			if (i == mMyCharacter->c_id)
			{
				mMyCharacter->killcount = packet->characterkillcount[i];
				mMyCharacter->deathcount = packet->characterdeathcount[i];
			}
			else {
				if (nullptr != mOtherCharacter[i])
				{
					mOtherCharacter[i]->killcount = packet->characterkillcount[i];
					mOtherCharacter[i]->deathcount = packet->characterdeathcount[i];
				}
			}
		}
		mMyCharacter->mInventory->mMainWidget->mScoreWidget->UpdateRank();

		break;
	}
	case SC_PACKET_GAMEWAITING: {
		//3초 기다리는 UI
		UE_LOG(LogTemp, Warning, TEXT("WAITING CALLED"));
		mMyCharacter->mLoadingWidget->RemoveFromParent();
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MWaitingWidget.MWaitingWidget_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		mMyCharacter->mWaitingWidget = CreateWidget<UUserWidget>(mMyCharacter->GetWorld(), WidgetClass);
		mMyCharacter->mWaitingWidget->AddToViewport();
		break;
	}
	case SC_PACKET_GAMESTART: {
		//waiting 위젯 지우고, 게임모드 바꿔주는
		UE_LOG(LogTemp, Warning, TEXT("START CALLED"));
		mMyCharacter->mWaitingWidget->RemoveFromParent();
		auto controller = mMyCharacter->GetWorld()->GetFirstPlayerController();
		mMyCharacter->mMainWidget->bActivate = true;
		FInputModeGameOnly gamemode;
		if (nullptr != controller)
		{
			controller->SetInputMode(gamemode);
			controller->SetShowMouseCursor(false);
		}
		mMyCharacter->mMainWidget->W_MiniMap_0->mSilverIconWidget->bTickActive = true;
		mMyCharacter->mMainWidget->W_MiniMap_0->mGoldIconWidget->bTickActive = true;
		mMyCharacter->mMainWidget->W_MiniMap_0->mBronzeIconWidget->bTickActive = true;
		break;
	}
	case SC_PACKET_GAMEEND: {
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MGameResultWidget.MGameResultWidget_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		auto GameResultWGT = CreateWidget<UGameResultWidget>(mMyCharacter->GetWorld(), WidgetClass);
		GameResultWGT->mScoreWidget = mMyCharacter->mInventory->mMainWidget->mScoreWidget;
		GameResultWGT->AddToViewport();			
		mMyCharacter->DisableInput(mMyCharacter->GetWorld()->GetFirstPlayerController());
		mMyCharacter->bAttacking = false;
		mMyCharacter->bLMBDown = false;
		break;

	}	
	case SC_PACKET_CHEAT_GAMETIME: {
		sc_packet_cheat_gametime* packet = reinterpret_cast<sc_packet_cheat_gametime*>(p);
		mMyCharacter->mMainWidget->fRemainTime = packet->milliseconds / 1000;
		break;
	}
	case SC_PACKET_SYNC_BANANA: {
		sc_packet_sync_banana* packet = reinterpret_cast<sc_packet_sync_banana*>(p);
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(mMyCharacter->GetWorld(), AProjectile::StaticClass(), actors);
		for (auto& actor : actors)
		{
			AProjectile* banana = Cast<AProjectile>(actor);
			if (nullptr != banana)
			{
				if (banana->_fType == 11)
				{
					if (banana->uniqueID == packet->bananaid)
					{
						banana->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
						banana->SetActorRotation(FRotator(packet->rx, packet->ry, packet->rz));
					}
				}
			}
		}
		break;
	}
	case SC_PACKET_KILL_INFO: {
		sc_packet_kill_info* packet = reinterpret_cast<sc_packet_kill_info*>(p);
		mMyCharacter->mMainWidget->UpdateKillLog(FString(packet->Attacker), FString(packet->Victim));
		break;
	}
	}
}









void Network::process_LobbyPacket(unsigned char* p)
{

	unsigned char Type = p[1];
	switch (Type) {
	case LC_PACKET_LOGIN_OK: {
		lc_packet_login_ok* packet = reinterpret_cast<lc_packet_login_ok*>(p);
		switch (packet->loginsuccess)
		{
		case 1: {
			if(mMyCharacter->mLoginWidget)
				mMyCharacter->mLoginWidget->RemoveFromParent();
			auto controller = mMyCharacter->GetWorld()->GetFirstPlayerController();
			mMyCharacter->mMainWidget->bActivate = true;
			mMyCharacter->CharacterName = FString(ANSI_TO_TCHAR(packet->name));
			mMyCharacter->Cash = packet->coin;
			mMyCharacter->skinType = packet->skintype;
			mMyCharacter->EquipSkin();
			MyCharacterName = mMyCharacter->CharacterName;
			bLoginFlag = true;
			bLevelOpenTriggerEnabled = false;	//openlevel로 인한 변경이 끝났으니 false로 바꿔줌.
			FInputModeGameOnly gamemode;
			if (nullptr != controller)
			{
				controller->SetInputMode(gamemode);
				controller->SetShowMouseCursor(false);
			}
			for (int i = 0; i < static_cast<int>(packet->numberofitemshave); ++i)
			{
				mMyCharacter->mMainWidget->W_Store->UpdateItemSlotStatus(static_cast<int>(packet->haveitems[i]));
			}
			break;
		}
		default: {
			FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MMessageBoxWidget.MMessageBoxWidget_C'"));
			auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
			auto MessageBoxWGT = CreateWidget<UMessageBoxWidget>(mMyCharacter->GetWorld(), WidgetClass);
			MessageBoxWGT->AddToViewport();
			MessageBoxWGT->MakeMessageBoxWithCode(packet->loginsuccess);
			break;
		}
		}
		break;
	}
	case LC_PACKET_MATCH_RESPONSE: {
		lc_packet_match_response* packet = reinterpret_cast<lc_packet_match_response*>(p);
		GameServerPort = packet->port;
		mMyCharacter->l_prev_size = 0;
		bLevelOpenTriggerEnabled = true;
		switch (packet->playertype)
		{
		case 0:
			UGameplayStatics::OpenLevel(mMyCharacter->GetWorld(), FName("FruitsPangPangMap_Player"));
			break;
		case 1:
			UGameplayStatics::OpenLevel(mMyCharacter->GetWorld(), FName("FruitsPangPangMap_AI"));
			break;
		}
		break;
	}
	case LC_PACKET_MATCH_UPDATE: {
		lc_packet_match_update* packet = reinterpret_cast<lc_packet_match_update*>(p);
		if (mMyCharacter->mMatchWidget != nullptr)
			mMyCharacter->mMatchWidget->UpdatePlayerCntText(packet->playercnt);
		break;
	}
	case LC_PACKET_SIGNUP_OK: {
		lc_packet_signup_ok* packet = reinterpret_cast<lc_packet_signup_ok*>(p);
		switch (packet->loginsuccess)
		{
		case 1: {
			FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MMessageBoxWidget.MMessageBoxWidget_C'"));
			auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
			auto MessageBoxWGT = CreateWidget<UMessageBoxWidget>(mMyCharacter->GetWorld(), WidgetClass);
			MessageBoxWGT->AddToViewport();
			MessageBoxWGT->MakeMessageBoxWithCode(2);	//회원가입 성공
			break;
		}
		default: {
			FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MMessageBoxWidget.MMessageBoxWidget_C'"));
			auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
			auto MessageBoxWGT = CreateWidget<UMessageBoxWidget>(mMyCharacter->GetWorld(), WidgetClass);
			MessageBoxWGT->AddToViewport();
			MessageBoxWGT->MakeMessageBoxWithCode(packet->loginsuccess);
			break;
		}
		}
		break;
	}
	case LC_PACKET_BUYITEM_RESULT: {
		lc_packet_buyitem_result* packet = reinterpret_cast<lc_packet_buyitem_result*>(p);
		mMyCharacter->Cash = packet->Coin;
		mMyCharacter->mMainWidget->W_Store->UpdateItemSlotStatus(packet->itemcode);
		mMyCharacter->mMainWidget->W_Store->UpdateCash(mMyCharacter->Cash);
		break;
	}
	case LC_PACKET_EQUIP_RESPONSE: {
		lc_packet_equip_response* packet = reinterpret_cast<lc_packet_equip_response*>(p);
		mMyCharacter->skinType = packet->itemcode;
		mMyCharacter->EquipSkin();
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
void CALLBACK recv_Gamecallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	WSA_OVER_EX* over = reinterpret_cast<WSA_OVER_EX*>(recv_over);

	if (nullptr == Network::GetNetwork()->mMyCharacter) return;

	int to_process_data = num_bytes + Network::GetNetwork()->mMyCharacter->_prev_size;
	unsigned char* packet = over->getBuf();
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Network::GetNetwork()->process_packet(packet);
		to_process_data -= packet_size;
		packet += packet_size;
		if (to_process_data > 0) packet_size = packet[0];
		else break;
	}
	Network::GetNetwork()->mMyCharacter->_prev_size = to_process_data;
	if (to_process_data > 0)
	{
		memcpy(over->getBuf(), packet, to_process_data);
	}
	Network::GetNetwork()->mMyCharacter->recvPacket();
}

void CALLBACK recv_Lobbycallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	WSA_OVER_EX* over = reinterpret_cast<WSA_OVER_EX*>(recv_over);

	if (nullptr == Network::GetNetwork()->mMyCharacter) return;
	if (INVALID_SOCKET == Network::GetNetwork()->mMyCharacter->l_socket) return;

	int to_process_data = num_bytes + Network::GetNetwork()->mMyCharacter->l_prev_size;
	unsigned char* packet = over->getBuf();
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Network::GetNetwork()->process_LobbyPacket(packet);
		to_process_data -= packet_size;
		packet += packet_size;
		if (to_process_data > 0) packet_size = packet[0];
		else break;
	}
	Network::GetNetwork()->mMyCharacter->l_prev_size = to_process_data;
	if (to_process_data > 0)
	{
		memcpy(over->getBuf(), packet, to_process_data);
	}
	Network::GetNetwork()->mMyCharacter->recvLobbyPacket();
}



void CALLBACK recv_Aicallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	WSA_OVER_EX* over = reinterpret_cast<WSA_OVER_EX*>(recv_over);
	//UE_LOG(LogTemp, Log, TEXT("recv_callback called"));
	if (0 > over->getId() || 8 < over->getId()) return;
	if (nullptr == Network::GetNetwork()->mAiCharacter[over->getId()]) return;
	int to_process_data = num_bytes + Network::GetNetwork()->mAiCharacter[over->getId()]->_prev_size;
	unsigned char* packet = over->getBuf();
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Network::GetNetwork()->process_Aipacket(static_cast<int>(over->getId()), packet);
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


void Network::process_Aipacket(int client_id, unsigned char* p)
{

	unsigned char packet_type = p[1];
	auto PacketOwner = Network::GetNetwork()->mAiCharacter[client_id];
	auto Game = Network::GetNetwork();
	switch (packet_type) {
	case SC_PACKET_LOGIN_OK: {
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(p);
		PacketOwner->c_id = packet->id;
		for (int i = 0; i < TREE_CNT; ++i)
		{
			if (nullptr != mTree[i])
			{
				mTree[i]->GenerateFruit(packet->TreeFruits[i]);
			}
		}
		for (int i = 0; i < PUNNET_CNT; ++i)
		{
			if (nullptr != mPunnet[i])
			{
				mPunnet[i]->GenerateFruit(packet->PunnetFruits[i]);
			}
		}
		for (int i = 0; i < HEAL_CNT; ++i)
		{
			if (nullptr != mHealSpawner[i])
			{
				mHealSpawner[i]->GenerateFruit(packet->HealFruits[i]);
			}
		}
		break;
	}
	case SC_PACKET_MOVE: {
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(p);
		int move_id = packet->id;

		//if Ai Move, dump packet
		//Ai가 움직이는 패킷은 버린다. 어차피 클라 내에서 움직이는거라 패킷으로 안움직여도 된다.
		//for문으로 하지말고, Map 구조 Key-value 구조를 사용하여 최적화 할 것.
		// ... 으나! 다른 버그가 터져서 결국 서버에서 판별하기로 결정...
		bool escape = false;
		if (0 != PacketOwner->overID) break;
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

		//UE_LOG(LogTemp, Log, TEXT("move called"));

		if (USER_START<= move_id && move_id < MAX_USER)
		{
			if (mOtherCharacter[move_id] != nullptr)
			{
				mOtherCharacter[move_id]->SetActorLocation(FVector(packet->x, packet->y, packet->z));
				mOtherCharacter[move_id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
				mOtherCharacter[move_id]->GroundSpeedd = packet->speed;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_MOVE id: %d"), move_id);
		}
		break;
	}
	case SC_PACKET_ANIM: {
		sc_packet_anim* packet = reinterpret_cast<sc_packet_anim*>(p);
		int anim_character_id = packet->id;

		//if Ai Move, dump packet
		//Ai가 움직이는 패킷은 버린다. 어차피 클라 내에서 움직이는거라 패킷으로 안움직여도 된다.
		bool escape = false;
		if (0 != PacketOwner->overID) break;
		for (auto ai : mAiCharacter)
		{
			if (ai == nullptr) continue;
			if (anim_character_id == ai->c_id)
			{
				escape = true;
				break;
			}
		}
		if (escape) break;
		switch (packet->animtype)
		{
			case static_cast<char>(Network::AnimType::Throw):{
				if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
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
				else {
					UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
				}
				break;
			}
			case static_cast<char>(Network::AnimType::PickSword_GreenOnion) :{
				if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
					if (mOtherCharacter[packet->id] != nullptr)
					{
						//UE_LOG(LogTemp, Log, TEXT("Pick Sword Anima"));
						UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
						if (AnimInstance && mOtherCharacter[packet->id]->PickSwordMontage)
						{
							AnimInstance->Montage_Play(mOtherCharacter[packet->id]->PickSwordMontage, 2.f);
							AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->PickSwordMontage);
							mOtherCharacter[packet->id]->SM_GreenOnion->SetHiddenInGame(false, false);

						}
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
				}
				break;
			}
			case static_cast<char>(Network::AnimType::PickSword_Carrot) : {
				if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
					if (mOtherCharacter[packet->id] != nullptr)
					{
						//UE_LOG(LogTemp, Log, TEXT("Pick Sword Anima"));
						UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
						if (AnimInstance && mOtherCharacter[packet->id]->PickSwordMontage)
						{
							AnimInstance->Montage_Play(mOtherCharacter[packet->id]->PickSwordMontage, 2.f);
							AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->PickSwordMontage);
							mOtherCharacter[packet->id]->SM_Carrot->SetHiddenInGame(false, false);

						}
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
				}
				break;
			}
			case static_cast<char>(Network::AnimType::DropSword) : {
				if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
					if (mOtherCharacter[packet->id] != nullptr)
					{
						mOtherCharacter[packet->id]->SM_GreenOnion->SetHiddenInGame(true, false);
						mOtherCharacter[packet->id]->SM_Carrot->SetHiddenInGame(true, false);
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
				}
				break;
			}
			case static_cast<char>(Network::AnimType::Slash) : {
				if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
					if (mOtherCharacter[packet->id] != nullptr)
					{
						UE_LOG(LogTemp, Log, TEXT("Slash Anima"));
						UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
						if (AnimInstance && mOtherCharacter[packet->id]->SlashMontage)
						{
							AnimInstance->Montage_Play(mOtherCharacter[packet->id]->SlashMontage, 2.f);
							AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->SlashMontage);

						}
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
				}
				break;
			}
			case static_cast<char>(Network::AnimType::Stab) : {
				if (USER_START <= anim_character_id && anim_character_id < MAX_USER) {
					if (mOtherCharacter[packet->id] != nullptr)
					{
						//UE_LOG(LogTemp, Log, TEXT("Slash Anima"));
						UAnimInstance* AnimInstance = mOtherCharacter[packet->id]->GetMesh()->GetAnimInstance();
						if (AnimInstance && mOtherCharacter[packet->id]->StabbingMontage)
						{
							AnimInstance->Montage_Play(mOtherCharacter[packet->id]->StabbingMontage, 2.f);
							AnimInstance->Montage_JumpToSection(FName("Default"), mOtherCharacter[packet->id]->StabbingMontage);

						}
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_ANIM id: %d"), anim_character_id);
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
		if (0 != PacketOwner->overID) break;
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

		if (USER_START <= id && id < MAX_USER)
		{
			if (nullptr != mOtherCharacter[id])
			{
				mOtherCharacter[id]->GetMesh()->SetVisibility(true);
				mOtherCharacter[id]->c_id = packet->id;
				mOtherCharacter[id]->s_connected = true;
			}
			else {
				FName path = TEXT("Blueprint'/Game/Character/BP_MyCharacter.BP_MyCharacter_C'"); //_C를 꼭 붙여야 된다고 함.
				UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
				FTransform trans(FQuat(packet->rx, packet->ry, packet->rz, packet->rw), FVector(10020, 12760, 300));
				auto mc = mAiCharacter[0]->GetWorld()->SpawnActorDeferred<AMyCharacter>(GeneratedInventoryBP, trans);
				if (nullptr != mc)
				{
					mc->SpawnDefaultController();
					mc->AutoPossessPlayer = EAutoReceiveInput::Disabled;
					mc->FinishSpawning(trans);
					mOtherCharacter[id] = mc;
					mOtherCharacter[id]->GetMesh()->SetVisibility(true);
					mOtherCharacter[id]->c_id = packet->id;
					mOtherCharacter[id]->s_connected = true;
				}
			}

		}
		else {
			UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_PUT_OBJECT id: %d"), id);
		}
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
		bool escape = false;
		if (0 != PacketOwner->overID) break;
		for (auto ai : mAiCharacter)
		{
			if (ai == nullptr) continue;
			if (other_id == ai->c_id)
			{
				escape = true;
				break;
			}
		}
		if (escape) break;

		mOtherCharacter[other_id]->ThrowInAIMode(FVector(packet->lx, packet->ly, packet->lz), FRotator(packet->rx, packet->ry, packet->rz),packet->fruitType, packet->uniqueid);



		//FTransform SocketTransform = FTransform(FQuat(packet->rx, packet->ry, packet->rz, packet->rw), FVector(packet->lx, packet->ly, packet->lz), FVector(packet->sx, packet->sy, packet->sz));
		////FName path = TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb_Test.Bomb_Test_C'"); //_C를 꼭 붙여야 된다고 함.
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
		//UE_LOG(LogTemp, Error, TEXT("slot Num : %d"), packet->slotNum);
		itemClass.Name = AInventory::ItemCodeToItemName(packet->itemCode);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(packet->itemCode);
		PacketOwner->mInventory->UpdateInventorySlot(itemClass, packet->itemAmount);
		int FruitAmount = PacketOwner->mInventory->mSlots[PacketOwner->SelectedHotKeySlotNum].Amount;
		AAIController* AIController = Cast<AAIController>(PacketOwner->Controller);
		if (nullptr != AIController)
			AIController->BrainComponent->GetBlackboardComponent()->SetValueAsInt(AAIController_Custom::AmountKey, FruitAmount);

		//sword_ai
		auto swordAIController = Cast<AAI_Sword_Controller_Custom>(PacketOwner->Controller);
		auto smartAIController = Cast<AAI_Smart_Controller_Custom>(PacketOwner->Controller);

		if (swordAIController && (packet->itemCode == 7 || packet->itemCode == 8)) // update when ItemCode is only 7 or 8
		{
			swordAIController->SavedItemCode = packet->itemCode;
			PacketOwner->PickSwordAnimation();
		}
		else if (smartAIController && (packet->itemCode == 7 || packet->itemCode == 8))
		{
			smartAIController->SavedItemCode = packet->itemCode;
			PacketOwner->PickSwordAnimation();
		}

		break;
	}
	case SC_PACKET_UPDATE_USERSTATUS: {
		sc_packet_update_userstatus* packet = reinterpret_cast<sc_packet_update_userstatus*>(p);
		PacketOwner->hp = packet->hp;
		//PacketOwner->mInventory->mMainWidget->UpdateHpBar();
		break;
	}
	case SC_PACKET_DIE: {
		sc_packet_die* packet = reinterpret_cast<sc_packet_die*>(p);
		//죽었을때 할 행동 ex) 죽은 ui, 죽은 Animation, 부활 ui
		//현재는 그냥 꺾어놓기만 했음.
		if (packet->id == PacketOwner->c_id) {

			PacketOwner->bIsDie = true;
			PacketOwner->GetCharacterMovement()->DisableMovement();
			//PacketOwner->bAttacking = false;
			//PacketOwner->OnAttackEnd.Broadcast();
			PacketOwner->SetActorEnableCollision(false);
		}
		else if (packet->id < MAX_USER)
		{
			if (mOtherCharacter[packet->id] != nullptr)
			{
				if (true == mOtherCharacter[packet->id]->s_connected)
				{
					mOtherCharacter[packet->id]->bIsDie = true;
					mOtherCharacter[packet->id]->GetCharacterMovement()->DisableMovement();
					mOtherCharacter[packet->id]->SetActorEnableCollision(false);
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
			
			PacketOwner->bIsDie = false;
			PacketOwner->bAttacking = false;
			PacketOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			PacketOwner->SetActorEnableCollision(true);
		}
		else if (packet->id < MAX_USER)
		{
			if (mOtherCharacter[packet->id] != nullptr)
			{
				if (true == mOtherCharacter[packet->id]->s_connected)
				{
					mOtherCharacter[packet->id]->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
					mOtherCharacter[packet->id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
					mOtherCharacter[packet->id]->GroundSpeedd = 0;

					mOtherCharacter[packet->id]->bIsDie = false;
					mOtherCharacter[packet->id]->SetActorEnableCollision(true);
				}
			}
		}
		break;
	}
	case SC_PACKET_UPDATE_INTERSTAT: {
		sc_packet_update_interstat* packet = reinterpret_cast<sc_packet_update_interstat*>(p);
		//BroadCast Packet just do once
		//BroadCast Packet은 딱 한번만 실행돼야 함.
		if (0 != PacketOwner->overID) break;
		if (packet->canHarvest)	//생성 로직
		{
			if (packet->useType == INTERACT_TYPE_TREE)
			{
				Game->mTree[packet->objNum]->GenerateFruit(packet->fruitType);
			}
			else if (packet->useType == INTERACT_TYPE_PUNNET)
			{
				//UE_LOG(LogTemp, Log, TEXT("Punnet Generate"));
				Game->mPunnet[packet->objNum]->GenerateFruit(packet->fruitType);
			}
			else if (packet->useType == INTERACT_TYPE_HEAL)
			{
				//UE_LOG(LogTemp, Log, TEXT("Heal Harvest"));
				Game->mHealSpawner[packet->objNum]->GenerateFruit(packet->fruitType);
			}
		}
		else {					//수확 로직

			if (packet->useType == INTERACT_TYPE_TREE)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
				//	FString::Printf(TEXT("My HP: %d "), Game->mTree[packet->objNum]->TreeId));
				Game->mTree[packet->objNum]->HarvestFruit();
			}
			else if (packet->useType == INTERACT_TYPE_PUNNET)
			{
				//UE_LOG(LogTemp, Log, TEXT("Punnet Harvest"));
				Game->mPunnet[packet->objNum]->HarvestFruit();
			}
			else if (packet->useType == INTERACT_TYPE_HEAL)
			{
				//UE_LOG(LogTemp, Log, TEXT("Heal Harvest"));
				Game->mHealSpawner[packet->objNum]->HarvestFruit();
			}

		}
		break;
	}
	case SC_PACKET_SYNC_BANANA: {
		sc_packet_sync_banana* packet = reinterpret_cast<sc_packet_sync_banana*>(p);
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(mAiCharacter[0]->GetWorld(), AProjectile::StaticClass(), actors);
		for (auto& actor : actors)
		{
			AProjectile* banana = Cast<AProjectile>(actor);
			if (nullptr != banana)
			{
				if (banana->_fType == 11)
				{
					if (banana->uniqueID == packet->bananaid)
					{
						banana->SetActorLocation(FVector(packet->lx, packet->ly, packet->lz));
						banana->SetActorRotation(FRotator(packet->rx, packet->ry, packet->rz));
					}
				}
			}
		}
		break;
	}
	case SC_PACKET_GAMEEND: {
		PacketOwner->GetController()->UnPossess();
		break;

	}
	default: {
		//Unknwon Packet Error
		break;
	}
	}
}
