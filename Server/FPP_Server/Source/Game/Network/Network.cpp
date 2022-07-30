#include <iostream>
#include <fstream>
#include <random>
#include "Network.h"
#include "../Object/Object.h"
#include "../Object/Character/Character.h"
#include "../Object/Interaction/Interaction.h"
#include "../Object/Interaction/Tree/Tree.h"
#include "../Object/Interaction/Punnet/Punnet.h"
#include "../Object/Interaction/Heal/Heal.h"
#include "../Object/Interaction/Banana/Banana.h"
#include "../Server/Server.h"

using namespace std;
HANDLE hiocp;
SOCKET s_socket;

std::array<Object*, MAX_OBJECT> objects;
Server* mServer;
Server* mDBServer;
std::atomic_int loginPlayerCnt = 0;
std::atomic_bool CheatGamePlayTime = false; //GamePlayTimeCheat must Played 1 Time 
concurrency::concurrent_priority_queue <Timer_Event> timer_queue;
concurrency::concurrent_queue <Log> logger;
const char* RandomAIName[] = { "Anderson","Allen","Adams","Brown","Baker","Bailey","Bell","Brooks","Clark","Collins","Davis","Evans","Flores","Howard","Garcia","Jones","Kelly",
"Miller","Martin","Nelson","Ortiz","Lewis","Phillips","Parker","Robinson","Rivera","Ross","Smith","Scott","Taylor","Turner","Wright","Ward" };

const int WorkerThreadsAmount = 6;

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

void FPP_LOG(const char* strLogFormat, ...)
{
	char buf[1000];
	Log t;
	va_list arg;
	va_start(arg, strLogFormat);
	ZeroMemory(buf, sizeof(buf));
	vsprintf_s(buf, strLogFormat, arg);
	va_end(arg);

	t.logtxt += buf;
	logger.push(t);
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
	for (int i = 0; i < MAX_USER; ++i)
	{
		auto user = reinterpret_cast<Character*>(objects[i]);
		user->state_lock.lock();
		if (Character::STATE::ST_FREE == user->_state)
		{
			user->_state = Character::STATE::ST_ACCEPT;
			user->state_lock.unlock();
			cout << i <<"번째 캐릭터 입장"<< endl;
			return i;
		}
		user->state_lock.unlock();
	}
	cout << "Player is Over the MAX_USER" << endl;
	return -1;
}

void Disconnect(int c_id)
{
	auto player = reinterpret_cast<Character*>(objects[c_id]);
	player->state_lock.lock();
	closesocket(player->_socket);
	player->_state = Character::STATE::ST_FREE;
	player->state_lock.unlock();
	for (auto& other : objects) {
		if (!other->isPlayer()) break;
		auto OtherPlayer = reinterpret_cast<Character*>(other);
		OtherPlayer->state_lock.lock();
		if (Character::STATE::ST_INGAME != OtherPlayer->_state) {
			OtherPlayer->state_lock.unlock();
			continue;
		}
		OtherPlayer->state_lock.unlock();
		send_remove_object_packet(OtherPlayer->_id, c_id);	//이거 언락위치 생각 
	}
}


void send_recycle_gameserver_packet()
{
	gl_packet_server_reset packet;
	packet.size = sizeof(packet);
	packet.type = GL_PACKET_SERVER_RESET;
	mServer->sendPacket(&packet, sizeof(packet));
}

void send_get_player_info_packet(const int& player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	gd_packet_get_player_info packet{};

	packet.size = sizeof(packet);
	packet.type = GD_PACKET_GET_PLAYER_INFO;
	strcpy_s(packet.name, player->name);
	packet.id = player_id;
	mDBServer->sendPacket(&packet, sizeof(packet));
}

void send_update_player_result(const int& player_id, const int& rank)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	if (1 == player->bAi) return;	//ai는 돈계산 X
	int Cash = player->mKillCount * 10;
	double adjust = 1.8 - (rank * 0.1);
	Cash = static_cast<int>(Cash * adjust);
	FPP_LOG("%s에게 최종 결산된 금액:%d", player->name, Cash);
	gd_packet_update_player_info packet{};


	packet.size = sizeof(packet);
	packet.type = GD_PACKET_UPDATE_PLAYER_INFO;
	strcpy_s(packet.name, player->name);
	packet.coin = Cash;
	mDBServer->sendPacket(&packet, sizeof(packet));
}


void send_login_ok_packet(int player_id, const char* playername)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_login_ok packet;
	memset(&packet, 0, sizeof(sc_packet_login_ok));

	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;

	for (int i = TREEID_START,tree = 0; i < TREEID_END; ++i,++tree)
	{		
		packet.TreeFruits[tree] = static_cast<char>(reinterpret_cast<Tree*>(objects[i])->_ftype);
	}

	for (int i = PUNNETID_START,punnet = 0; i < PUNNETID_END; ++i,++punnet)
	{
		packet.PunnetFruits[punnet] = static_cast<char>(reinterpret_cast<Punnet*>(objects[i])->_ftype);
	}

	for (int i = HEALID_START, heal = 0; i < HEALID_END; ++i, ++heal)
	{
		packet.HealFruits[heal] = static_cast<char>(reinterpret_cast<Heal*>(objects[i])->_ftype);
	}
	packet.skintype = player->skintype;
	packet.id = player_id;
	strcpy_s(packet.name, playername);
	player->sendPacket(&packet, sizeof(packet));
}

void send_move_packet(int player_id, int mover_id, float value)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_move packet{};
	packet.id = mover_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_MOVE;
	packet.x = objects[mover_id]->x;
	packet.y = objects[mover_id]->y;
	packet.z = objects[mover_id]->z;
	packet.rx = objects[mover_id]->rx;
	packet.ry = objects[mover_id]->ry;
	packet.rz = objects[mover_id]->rz;
	packet.rw = objects[mover_id]->rw;
	packet.speed = value;
	player->sendPacket(&packet, sizeof(packet));
}

void send_anim_packet(int player_id, int animCharacter_id, char animtype)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_anim packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_ANIM;
	packet.id = animCharacter_id;
	packet.animtype = animtype;
	player->sendPacket(&packet, sizeof(packet));
}

void send_throwfruit_packet(const int& thrower_character_id, const int& other_character_id,
	const float& rx, const float& ry, const float& rz, const float& rw,	//rotate
	const float& lx, const float& ly, const float& lz,	//location
	const float& sx, const float& sy, const float& sz,	//scale
	const int& fruittype,	//item code
	const int& uniqueid	//item unique id ( using banana sync )
)
{
	auto player = reinterpret_cast<Character*>(objects[other_character_id]);
	sc_packet_spawnobj packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_SPAWNOBJ;
	packet.id = thrower_character_id;
	packet.rx = rx, packet.ry = ry, packet.rz = rz, packet.rw = rw;
	packet.lx = lx, packet.ly = ly, packet.lz = lz;
	packet.sx = sx, packet.sy = sy, packet.sz = sz;
	packet.fruitType = fruittype;
	packet.uniqueid = uniqueid;
	player->sendPacket(&packet, sizeof(packet));
}


void send_update_inventory_packet(int player_id, short slotNum)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_update_inventory packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_UPDATE_INVENTORY;
	packet.slotNum = slotNum;
	packet.itemCode = static_cast<short>(player->mSlot[slotNum].type);
	packet.itemAmount = player->mSlot[slotNum].amount;
	player->sendPacket(&packet, sizeof(packet));

}

void send_update_interstat_packet(const int& player_id, const int& object_id, const bool& CanHarvest, const int& interactType, const int& FruitType)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_update_interstat packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_UPDATE_INTERSTAT;
	packet.useType = interactType;
	packet.objNum = object_id;
	packet.canHarvest = CanHarvest;
	packet.fruitType = FruitType;
	player->sendPacket(&packet, sizeof(packet));
}


void send_remove_object_packet(int player_id, int removeCharacter_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_remove_object packet{};
	packet.id = removeCharacter_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_REMOVE_OBJECT;
	player->sendPacket(&packet, sizeof(packet));
}

void send_update_userstatus_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_update_userstatus packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_UPDATE_USERSTATUS;
	packet.hp = player->hp;

	player->sendPacket(&packet, sizeof(packet));
}

void send_die_packet(int player_id,int deadplayer_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_die packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_DIE;
	packet.id = deadplayer_id;
	player->sendPacket(&packet, sizeof(packet));
}

void send_respawn_packet(int player_id, int respawner_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	auto respawner = objects[respawner_id];
	sc_packet_respawn packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_RESPAWN;
	packet.id = respawner_id;
	packet.lx = respawner->x;
	packet.ly = respawner->y;
	packet.lz = respawner->z;
	packet.rx = respawner->rx;
	packet.ry = respawner->ry;
	packet.rz = respawner->rz;
	packet.rw = respawner->rw;

	FPP_LOG("User[%d]의 리스폰을 User[%s]에게 알립니다.", respawner_id, player->name);
	player->sendPacket(&packet, sizeof(packet));
}

void send_update_score_packet(int player_id,short* userdeathcount, short* userkillcount)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_update_score packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_UPDATE_SCORE;
	packet.id = player_id;
	memcpy(packet.characterdeathcount, userdeathcount, sizeof(packet.characterdeathcount));
	memcpy(packet.characterkillcount, userkillcount, sizeof(packet.characterkillcount));

	player->sendPacket(&packet, sizeof(packet));
}
void send_gamewaiting_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_gamewaiting packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_GAMEWAITING;
	player->sendPacket(&packet, sizeof(packet));
}


void send_gamestart_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_gamestart packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_GAMESTART;
	player->sendPacket(&packet, sizeof(packet));
}

void send_gameend_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_gameend packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_GAMEEND;
	player->sendPacket(&packet, sizeof(packet));
}

void send_cheat_changegametime_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_cheat_gametime packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_CHEAT_GAMETIME;
	packet.milliseconds = GAMEPLAYTIME_CHEAT_MILLI;
	player->sendPacket(&packet, sizeof(packet));
}

void send_sync_banana(const int& player_id, 
	const float& rx, const float& ry, const float& rz, const float& rw,
	const float& lx, const float& ly, const float& lz,
	const int& uniqueid)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_sync_banana packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_SYNC_BANANA;
	packet.rx = rx, packet.ry = ry, packet.rz = rz, packet.rw = rw;
	packet.lx = lx, packet.ly = ly, packet.lz = lz;
	packet.bananaid = uniqueid;
	player->sendPacket(&packet, sizeof(packet));
}

void send_kill_info_packet(const int& player_id, const int& attacker_id, const int& victim_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	auto attacker = reinterpret_cast<Character*>(objects[attacker_id]);
	auto victim = reinterpret_cast<Character*>(objects[victim_id]);
	sc_packet_kill_info packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_KILL_INFO;
	strcpy_s(packet.Attacker, attacker->name);
	strcpy_s(packet.Victim, victim->name);
	player->sendPacket(&packet, sizeof(packet));
}

void send_step_banana_packet(const int& player_id, const int& falldown_id, const int& banana_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_step_banana packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_STEP_BANANA;
	packet.clientid = falldown_id;
	packet.bananaid = banana_id;
	player->sendPacket(&packet, sizeof(packet));
}

void process_packet(int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	Object* object = objects[client_id];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);
		Character* character = reinterpret_cast<Character*>(object);

		strcpy_s(character->name, packet->name);
		character->bAi = packet->cType;
		//Ai는 이름을 랜덤으로 부여받음. AI는 스킨이 없기때문에 바로 로그인 ok이지만,
		//플레이어블 캐릭터는 스킨 정보를 받아와야하기때문에 DB를 한번 거침.
		if (packet->cType == 1)
		{
			random_device rd;
			mt19937 rng(rd());
			uniform_int_distribution<int> randName(0, 32);
			int randCnt = randName(rng);
			strcpy_s(character->name, RandomAIName[randCnt]);
		

			send_login_ok_packet(client_id, character->name);

			FPP_LOG("플레이어[%s] 접속", character->name);

			for (auto& other : objects) {
				if (!other->isPlayer()) break;
				if (other->_id == client_id) continue;

				auto OtherPlayer = reinterpret_cast<Character*>(other);
				if (character->bAi && OtherPlayer->bAi) continue;

				OtherPlayer->state_lock.lock();
				if (Character::STATE::ST_INGAME != OtherPlayer->_state) {
					OtherPlayer->state_lock.unlock();
					continue;
				}
				OtherPlayer->state_lock.unlock();

				sc_packet_put_object packet;
				packet.id = client_id;
				strcpy_s(packet.name, character->name);
				packet.object_type = 0;
				packet.size = sizeof(packet);
				packet.type = SC_PACKET_PUT_OBJECT;
				packet.x = character->x;
				packet.y = character->y;
				packet.z = character->z;
				packet.rx = character->rx;
				packet.ry = character->ry;
				packet.rz = character->rz;
				packet.rw = character->rw;
				packet.skintype = 0;
				OtherPlayer->sendPacket(&packet, sizeof(packet));
			}

			for (auto& other : objects) {
				if (!other->isPlayer()) break;
				if (other->_id == client_id) continue;
				auto OtherPlayer = reinterpret_cast<Character*>(other);
				if (character->bAi && OtherPlayer->bAi) continue;
				OtherPlayer->state_lock.lock();
				if (Character::STATE::ST_INGAME != OtherPlayer->_state) {
					OtherPlayer->state_lock.unlock();
					continue;
				}
				OtherPlayer->state_lock.unlock();

				sc_packet_put_object packet;
				packet.id = OtherPlayer->_id;
				strcpy_s(packet.name, OtherPlayer->name);
				packet.object_type = 0;
				packet.size = sizeof(packet);
				packet.type = SC_PACKET_PUT_OBJECT;
				packet.x = OtherPlayer->x;
				packet.y = OtherPlayer->y;
				packet.skintype = 0;

				character->sendPacket(&packet, sizeof(packet));
			}

			character->state_lock.lock();
			character->_state = Character::STATE::ST_INGAME;
			character->state_lock.unlock();


		}
		else {
			send_get_player_info_packet(client_id);
		}

		break;
	}
	case CS_PACKET_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p);
		Character* character = reinterpret_cast<Character*>(object);
		object->x = packet->x;
		object->y = packet->y;
		object->z = packet->z;
		object->rx = packet->rx;
		object->ry = packet->ry;
		object->rz = packet->rz;
		object->rw = packet->rw;
		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			if (other->_id == client_id) continue;
			auto OtherPlayer = reinterpret_cast<Character*>(other);
			if (character->bAi && OtherPlayer->bAi) continue;
			OtherPlayer->state_lock.lock();
			if (Character::STATE::ST_INGAME == OtherPlayer->_state)
			{
				OtherPlayer->state_lock.unlock();
				send_move_packet(OtherPlayer->_id, client_id, packet->speed);
			}
			else OtherPlayer->state_lock.unlock();
		}
		break;
	}
	case CS_PACKET_ANIM: {
		cs_packet_anim* packet = reinterpret_cast<cs_packet_anim*>(p);
		//cout << client_id << endl;


		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			if (other->_id == client_id) continue;
			auto character = reinterpret_cast<Character*>(other);

			character->state_lock.lock();
			if (Character::STATE::ST_INGAME == character->_state)
			{
				character->state_lock.unlock();
				send_anim_packet(character->_id, client_id, packet->animtype);
			}
			else character->state_lock.unlock();
		}


		break;
	}
	case CS_PACKET_SPAWNITEMOBJ: {
		cs_packet_spawnitemobj* packet = reinterpret_cast<cs_packet_spawnitemobj*>(p);
		Character* character = reinterpret_cast<Character*>(object);
		if (packet->itemSlotNum < 0)
		{
			cout << "유효하지 않은 item Slot 입니다 : " << packet->itemSlotNum << endl;
			FPP_LOG("User[%d] 유효하지 않은 item Slot %d", client_id, packet->itemSlotNum);
			break;
		}

		if (character->mSlot[packet->itemSlotNum].amount > 0)
		{
			character->mSlot[packet->itemSlotNum].amount -= 1;
			cout << client_id << "번째 유저의" << packet->itemSlotNum << "번째 슬롯 아이템 1개 감소 현재 개수:" << character->mSlot[packet->itemSlotNum].amount << endl;
			FPP_LOG("[%d]유저 [%d]번째 슬롯 아이템 1개 감소 현재 개수: %d", client_id, packet->itemSlotNum, character->mSlot[packet->itemSlotNum].amount);
		}
		else {
			FPP_LOG("[%d]번째 유저가 %d 번째 슬롯의 아이템이 없는데 사용하려고 시도함.", client_id, packet->itemSlotNum);
			break;
		}
		int uniqueID = 0;
		if (packet->fruitType == static_cast<int>(FRUITTYPE::T_BANANA))
		{
			//바나나 오브젝트 풀에서 꺼내기
			for (int i = BANANAID_START; i < BANANAID_END; ++i)
			{
				auto banana = reinterpret_cast<Banana*>(objects[i]);
				banana->state_lock.lock();
				if (banana->_state == Banana::STATE::ST_FREE)
				{
					banana->_state = Banana::STATE::ST_ACTIVE;
					banana->state_lock.unlock();
					uniqueID = banana->_id;
					break;	//꺼내는데 성공하면 더이상 반복문 돌 필요 없음. 바로 탈출
				}
				else {
					banana->state_lock.unlock();
				}
			}

			for (auto& other : objects) {
				if (!other->isPlayer()) break;
				//if (other->_id == client_id) continue; 바나나는 자기 자신한테도 패킷을 보내야함. 무조건 서버 검증을 거치고 던지게 끔 설계
				auto OtherPlayer = reinterpret_cast<Character*>(other);
				if (character->bAi && OtherPlayer->bAi) continue;
				OtherPlayer->state_lock.lock();
				if (Character::STATE::ST_INGAME == OtherPlayer->_state)
				{
					OtherPlayer->state_lock.unlock();
					send_throwfruit_packet(client_id, OtherPlayer->_id,
						packet->rx, packet->ry, packet->rz, packet->rw,
						packet->lx, packet->ly, packet->lz,
						packet->sx, packet->sy, packet->sz,
						packet->fruitType, uniqueID);
				}
				else OtherPlayer->state_lock.unlock();
			}

		}
		else {
			for (auto& other : objects) {
				if (!other->isPlayer()) break;
				if (other->_id == client_id) continue;
				auto OtherPlayer = reinterpret_cast<Character*>(other);
				if (character->bAi && OtherPlayer->bAi) continue;
				OtherPlayer->state_lock.lock();
				if (Character::STATE::ST_INGAME == OtherPlayer->_state)
				{
					OtherPlayer->state_lock.unlock();
					send_throwfruit_packet(client_id, OtherPlayer->_id,
						packet->rx, packet->ry, packet->rz, packet->rw,
						packet->lx, packet->ly, packet->lz,
						packet->sx, packet->sy, packet->sz,
						packet->fruitType, uniqueID);
				}
				else OtherPlayer->state_lock.unlock();
			}
		}
		
		break;
	}
	case CS_PACKET_GETFRUITS_TREE: {
		cs_packet_getfruits* packet = reinterpret_cast<cs_packet_getfruits*>(p);
		Tree* tree = reinterpret_cast<Tree*>(objects[packet->obj_id + TREEID_START]);
		tree->interact(object);
		break;
	}
	case CS_PACKET_GETFRUITS_PUNNET: {
		cs_packet_getfruits* packet = reinterpret_cast<cs_packet_getfruits*>(p);
		Punnet* punnet = reinterpret_cast<Punnet*>(objects[packet->obj_id + PUNNETID_START]);		
		punnet->interact(object);
		break;
	}
	case CS_PACKET_GETFRUITS_HEAL: {
		cs_packet_getfruits* packet = reinterpret_cast<cs_packet_getfruits*>(p);
		Heal* heal = reinterpret_cast<Heal*>(objects[packet->obj_id + HEALID_START]);
		heal->interact(object);
		break;
	}
	case CS_PACKET_STEP_BANANA: {
		cs_packet_step_banana* packet = reinterpret_cast<cs_packet_step_banana*>(p);
		Banana* banana = reinterpret_cast<Banana*>(objects[packet->bananaid]);
		banana->interact(object);
		break;
	}
	case CS_PACKET_HIT: {
		cs_packet_hit* packet = reinterpret_cast<cs_packet_hit*>(p);
		Character* character = reinterpret_cast<Character*>(object);
		//cout << "HurtBy ID:" << packet->attacker_id << endl;
		if (!(USER_START <= packet->attacker_id && packet->attacker_id < MAX_USER)) break;
		character->HurtBy(packet->fruitType, packet->attacker_id);
		break;
	}
	case CS_PACKET_POS: {
		cs_packet_pos* packet = reinterpret_cast<cs_packet_pos*>(p);

		switch (packet->useType)
		{
		case POS_TYPE_DURIAN: {

			cout << "위치 : 네트워크 패킷" << packet->x << "," << packet->y << "," << packet->z  << endl;
			Timer_Event instq;
			instq.x = static_cast<int>(packet->x);
			instq.y = static_cast<int>(packet->y);
			instq.z = static_cast<int>(packet->z);
			instq.object_id = 50;	//터지는 횟수
			instq.player_id = client_id;	//터트린 사람(공격자)
			instq.type = Timer_Event::TIMER_TYPE::TYPE_DURIAN_DMG;
			instq.exec_time = chrono::system_clock::now() + 200ms;
			timer_queue.push(instq);
			break;
		}
		}
		break;
	}
	case CS_PACKET_SELECT_RESPAWN: {
		cs_packet_select_respawn* packet = reinterpret_cast<cs_packet_select_respawn*>(p);

		Character* RespawnPlayer = reinterpret_cast<Character*>(objects[client_id]);
		if (RespawnPlayer->hp > 0)
		{
			//break;
		}
		RespawnPlayer->Respawn(packet->numbering);
		cout << "플레이어 " << client_id << "리스폰\n";

		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			auto character = reinterpret_cast<Character*>(other);

			character->state_lock.lock();
			if (Character::STATE::ST_INGAME == character->_state)
			{
				character->state_lock.unlock();
				send_respawn_packet(character->_id, RespawnPlayer->_id);
				if (character->_id == RespawnPlayer->_id)
				{
					send_update_userstatus_packet(RespawnPlayer->_id);
				}
			}
			else character->state_lock.unlock();
		}
		break;
	}
	case CS_PACKET_PREGAMESETTINGCOMPLETE: {
		cs_packet_pregamesettingcomplete* packet = reinterpret_cast<cs_packet_pregamesettingcomplete*>(p);

		loginPlayerCnt++;
		if (loginPlayerCnt == MAX_PLAYER_CONN)
		{
			cout << " gogo" << endl;
			Timer_Event instq;
			instq.type = Timer_Event::TIMER_TYPE::TYPE_GAME_WAIT;
			instq.exec_time = chrono::system_clock::now() + 3000ms;
			timer_queue.push(instq);
		}
		break;
	}
	case CS_PACKET_CHEAT: {
		cs_packet_cheat* packet = reinterpret_cast<cs_packet_cheat*>(p);
		switch (packet->cheatType)
		{
		case CHEAT_TYPE_GAMETIME: {
			if (CheatGamePlayTime)
				break;
			CheatGamePlayTime = true;

			//reset Game Play Time
			Timer_Event instq;
			instq.type = Timer_Event::TIMER_TYPE::TYPE_GAME_END;
			instq.exec_time = chrono::system_clock::now() + chrono::milliseconds(20000);
			timer_queue.push(instq);

			//게임플레이시간이 바뀐걸 모든 유저에게 알린다.
			for (auto& other : objects) {
				if (!other->isPlayer()) break;
				auto character = reinterpret_cast<Character*>(other);

				character->state_lock.lock();
				if (Character::STATE::ST_INGAME == character->_state)
				{
					character->state_lock.unlock();
					send_cheat_changegametime_packet(character->_id);
				}
				else character->state_lock.unlock();
			}

			break;
		}
		case CHEAT_TYPE_GIVEITEM: {
			auto character = reinterpret_cast<Character*>(object);
			switch (packet->itemType)
			{
			case static_cast<char>(FRUITTYPE::T_TOMATO):
			case static_cast<char>(FRUITTYPE::T_KIWI):
			case static_cast<char>(FRUITTYPE::T_APPLE):
				character->UpdateInventorySlotAtIndex(0, static_cast<FRUITTYPE>(packet->itemType), 1);
				break;
			case static_cast<char>(FRUITTYPE::T_WATERMELON):
			case static_cast<char>(FRUITTYPE::T_PINEAPPLE):
			case static_cast<char>(FRUITTYPE::T_PUMPKIN):
				character->UpdateInventorySlotAtIndex(1, static_cast<FRUITTYPE>(packet->itemType), 1);
				break;
			case static_cast<char>(FRUITTYPE::T_GREENONION):
			case static_cast<char>(FRUITTYPE::T_CARROT):
				character->UpdateInventorySlotAtIndex(2, static_cast<FRUITTYPE>(packet->itemType), 1);
				break;
			case static_cast<char>(FRUITTYPE::T_DURIAN):
			case static_cast<char>(FRUITTYPE::T_NUT):
				character->UpdateInventorySlotAtIndex(3, static_cast<FRUITTYPE>(packet->itemType), 1);
				break;
			case static_cast<char>(FRUITTYPE::T_BANANA):
				character->UpdateInventorySlotAtIndex(4, static_cast<FRUITTYPE>(packet->itemType), 1);
				break;
			}
			break;
		}
		}
		break;
	}
	case CS_PACKET_SYNC_BANANA: {
		cs_packet_sync_banana* packet = reinterpret_cast<cs_packet_sync_banana*>(p);
		Character* character = reinterpret_cast<Character*>(object);

		int uniqueID = packet->bananaid;

		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			if (other->_id == client_id) continue;
			auto OtherPlayer = reinterpret_cast<Character*>(other);
			if (character->bAi && OtherPlayer->bAi) continue;
			OtherPlayer->state_lock.lock();
			if (Character::STATE::ST_INGAME == OtherPlayer->_state)
			{
				OtherPlayer->state_lock.unlock();
				send_sync_banana(OtherPlayer->_id,
					packet->rx, packet->ry, packet->rz, packet->rw,
					packet->lx, packet->ly, packet->lz,
					uniqueID
				);
			}
			else OtherPlayer->state_lock.unlock();
		}
		break;
	}
	}
}

void process_packet_for_Server(unsigned char* p)
{

	unsigned char packet_type = p[1];

	switch (packet_type) {
	case LG_PACKET_LOGIN_OK: {
		lg_packet_login_ok* packet = reinterpret_cast<lg_packet_login_ok*>(p);


		cout << "로비서버와 연결 완료\n";

		lg_packet_login_ok spacket;
		memset(&spacket, 0, sizeof(spacket));
		spacket.size = sizeof(spacket);
		spacket.type = LG_PACKET_LOGIN_OK;
		mServer->sendPacket(&spacket, sizeof(spacket));

		break;
	}
	case DG_PACKET_REQUEST_PLAYER_INFO: {
		dg_packet_request_player_info* packet = reinterpret_cast<dg_packet_request_player_info*>(p);
		Character* character = reinterpret_cast<Character*>(objects[packet->id]);
		character->skintype = packet->skintype;

		send_login_ok_packet(packet->id, character->name);

		FPP_LOG("플레이어[%s] 접속", character->name);

		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			if (other->_id == packet->id) continue;

			auto OtherPlayer = reinterpret_cast<Character*>(other);
			if (character->bAi && OtherPlayer->bAi) continue;

			OtherPlayer->state_lock.lock();
			if (Character::STATE::ST_INGAME != OtherPlayer->_state) {
				OtherPlayer->state_lock.unlock();
				continue;
			}
			OtherPlayer->state_lock.unlock();

			sc_packet_put_object sendpacket;
			sendpacket.id = packet->id;
			strcpy_s(sendpacket.name, character->name);
			sendpacket.object_type = 0;
			sendpacket.size = sizeof(sendpacket);
			sendpacket.type = SC_PACKET_PUT_OBJECT;
			sendpacket.x = character->x;
			sendpacket.y = character->y;
			sendpacket.z = character->z;
			sendpacket.rx = character->rx;
			sendpacket.ry = character->ry;
			sendpacket.rz = character->rz;
			sendpacket.rw = character->rw;
			sendpacket.skintype = character->skintype;
			OtherPlayer->sendPacket(&sendpacket, sizeof(sendpacket));
		}

		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			if (other->_id == packet->id) continue;
			auto OtherPlayer = reinterpret_cast<Character*>(other);
			if (character->bAi && OtherPlayer->bAi) continue;
			OtherPlayer->state_lock.lock();
			if (Character::STATE::ST_INGAME != OtherPlayer->_state) {
				OtherPlayer->state_lock.unlock();
				continue;
			}
			OtherPlayer->state_lock.unlock();

			sc_packet_put_object sendpacket;
			sendpacket.id = OtherPlayer->_id;
			strcpy_s(sendpacket.name, OtherPlayer->name);
			sendpacket.object_type = 0;
			sendpacket.size = sizeof(sendpacket);
			sendpacket.type = SC_PACKET_PUT_OBJECT;
			sendpacket.x = OtherPlayer->x;
			sendpacket.y = OtherPlayer->y;
			sendpacket.skintype = OtherPlayer->skintype;

			character->sendPacket(&sendpacket, sizeof(sendpacket));
		}

		character->state_lock.lock();
		character->_state = Character::STATE::ST_INGAME;
		character->state_lock.unlock();
	}
	}
}

void ResetGame()
{
	CheatGamePlayTime = false;
	loginPlayerCnt = 0;
}
