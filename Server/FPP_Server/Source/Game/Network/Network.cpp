#include <iostream>
#include <fstream>
#include <random>
#include "Network.h"
#include "../Object/Object.h"
#include "../Object/Character/Character.h"
#include "../Object/Interaction/Interaction.h"
#include "../Object/Interaction/Tree/Tree.h"
#include "../Object/Interaction/Punnet/Punnet.h"

using namespace std;
HANDLE hiocp;
SOCKET s_socket;

std::array<Object*, MAX_OBJECT> objects;
std::atomic_int loginPlayerCnt;
std::atomic_bool GameActive = true;
std::atomic_bool CheatGamePlayTime = false; //GamePlayTimeCheat must Played 1 Time 
concurrency::concurrent_priority_queue <Timer_Event> timer_queue;
concurrency::concurrent_queue <Log> logger;

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
	static int g_id = 0;
	for (int i = 0; i < MAX_USER; ++i)
	{
		auto user = reinterpret_cast<Character*>(objects[i]);
		user->state_lock.lock();
		if (Character::STATE::ST_FREE == user->_state)
		{
			user->_state = Character::STATE::ST_ACCEPT;
			user->state_lock.unlock();
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


void send_login_ok_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_login_ok packet;
	memset(&packet, 0, sizeof(sc_packet_login_ok));

	for (int i = TREEID_START,tree = 0; i < TREEID_END; ++i,++tree)
	{		
		packet.TreeFruits[tree] = static_cast<char>(reinterpret_cast<Tree*>(objects[i])->_ftype);
	}

	for (int i = PUNNETID_START,punnet = 0; i < PUNNETID_END; ++i,++punnet)
	{
		packet.PunnetFruits[punnet] = static_cast<char>(reinterpret_cast<Punnet*>(objects[i])->_ftype);
	}

	packet.id = player_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	player->sendPacket(&packet, sizeof(packet));
}

void send_move_packet(int player_id, int mover_id, float value)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_move packet;
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
	sc_packet_anim packet;
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
	sc_packet_spawnobj packet;
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
	sc_packet_update_inventory packet;
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
	sc_packet_update_interstat packet;
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
	sc_packet_remove_object packet;
	packet.id = removeCharacter_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_REMOVE_OBJECT;
	player->sendPacket(&packet, sizeof(packet));
}

void send_update_userstatus_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_update_userstatus packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_UPDATE_USERSTATUS;
	packet.hp = player->hp;

	player->sendPacket(&packet, sizeof(packet));
}

void send_die_packet(int player_id,int deadplayer_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_die packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_DIE;
	packet.id = deadplayer_id;
	player->sendPacket(&packet, sizeof(packet));
}

void send_respawn_packet(int player_id, int respawner_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	auto respawner = objects[respawner_id];
	sc_packet_respawn packet;
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

	FPP_LOG("User[%d]의 리스폰을 User[%d]에게 알립니다.", respawner_id, player_id);
	player->sendPacket(&packet, sizeof(packet));
}

void send_update_score_packet(int player_id,short* userdeathcount, short* userkillcount)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_update_score packet;
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
	sc_packet_gamewaiting packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_GAMEWAITING;
	player->sendPacket(&packet, sizeof(packet));
}


void send_gamestart_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_gamestart packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_GAMESTART;
	player->sendPacket(&packet, sizeof(packet));
}

void send_gameend_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_gameend packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_GAMEEND;
	player->sendPacket(&packet, sizeof(packet));
}

void send_cheat_changegametime_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_cheat_gametime packet;
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
	sc_packet_sync_banana packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_SYNC_BANANA;
	packet.rx = rx, packet.ry = ry, packet.rz = rz, packet.rw = rw;
	packet.lx = lx, packet.ly = ly, packet.lz = lz;
	packet.bananaid = uniqueid;
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
		send_login_ok_packet(client_id);
		FPP_LOG("플레이어[%d] 접속", client_id);

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
			//strcpy_s(packet.name, OtherPlayer->name);
			packet.object_type = 0;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.x = OtherPlayer->x;
			packet.y = OtherPlayer->y;

			character->sendPacket(&packet, sizeof(packet));
		}

		character->state_lock.lock();
		character->_state = Character::STATE::ST_INGAME;
		character->state_lock.unlock();
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
			uniqueID = character->_id * 10000000 + packet->uniquebananaid;
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
		break;
	}
	case CS_PACKET_GETFRUITS_TREE: {
		cs_packet_getfruits* packet = reinterpret_cast<cs_packet_getfruits*>(p);
		Character* character = reinterpret_cast<Character*>(object);
		Tree* tree = reinterpret_cast<Tree*>(objects[packet->obj_id + TREEID_START]);
		
		if (!tree->canHarvest)
		{
			cout << " 수확할 수 없습니다!" << endl;
			break;

		}
		tree->canHarvest = false;

		//cout << "과일 받았습니다(나무)" << packet->obj_id << endl;
		FPP_LOG("플레이어[%d]가 [%d]번째 나무에서 [%d]타입의 아이템 획득", client_id, tree->_id, tree->_ftype);
		switch (tree->_ttype)
		{
		case TREETYPE::GREEN:
			character->UpdateInventorySlotAtIndex(0, tree->_ftype, 10);
			send_update_inventory_packet(client_id, 0);
			break;
		case TREETYPE::ORANGE:
			character->UpdateInventorySlotAtIndex(1, tree->_ftype, 5);
			send_update_inventory_packet(client_id, 1);
			break;
		}
		tree->interact();

		for (auto& other : objects)
		{
			if (!other->isPlayer()) break;
			auto player = reinterpret_cast<Character*>(other);
			if (player->_state == Character::STATE::ST_INGAME)
			{
				//cout << "과일나무 떨어졌다고 보냅니다"<<packet->obj_id<<"," << endl;
				send_update_interstat_packet(other->_id, packet->obj_id, false, INTERACT_TYPE_TREE);
			}
		}
		/*}
		tree->CanHarvestLock.unlock();
		*/

		break;
	}
	case CS_PACKET_GETFRUITS_PUNNET: {
		cs_packet_getfruits* packet = reinterpret_cast<cs_packet_getfruits*>(p);
		Character* character = reinterpret_cast<Character*>(object);
		Punnet* punnet = reinterpret_cast<Punnet*>(objects[packet->obj_id + PUNNETID_START]);

		if (!punnet->canHarvest)
			break;
		punnet->canHarvest = false;

		//cout << "과일 받았습니다(과일상자)" << endl;
		if (punnet->_ftype == FRUITTYPE::T_HEAL)
		{
			character->Heal(10);
		}
		else if (punnet->_ftype == FRUITTYPE::T_GREENONION)
		{
			character->UpdateInventorySlotAtIndex(2, punnet->_ftype, 1);
		}
		else if (punnet->_ftype == FRUITTYPE::T_CARROT)
		{
			character->UpdateInventorySlotAtIndex(2, punnet->_ftype, 1);
		}
		else if (punnet->_ftype == FRUITTYPE::T_BANANA)
		{
			character->UpdateInventorySlotAtIndex(4, punnet->_ftype, 3);
		}
		else
		{
			character->UpdateInventorySlotAtIndex(3, punnet->_ftype, 5);
		}

		
		punnet->interact();

		for (auto& other : objects)
		{
			if (!other->isPlayer()) break;
			auto player = reinterpret_cast<Character*>(other);
			if (player->_state == Character::STATE::ST_INGAME)
			{
				//cout << "과일박스 먹었다고 보냅니다" <<packet->obj_id<<"," << endl;
				send_update_interstat_packet(other->_id, packet->obj_id, false, INTERACT_TYPE_PUNNET);
			}
		}
		break;
	}
	case CS_PACKET_USEITEM: {

		cs_packet_useitem* packet = reinterpret_cast<cs_packet_useitem*>(p);
		Character* character = reinterpret_cast<Character*>(object);
		if (character->mSlot[character->mActivationSlot].amount > 0)
		{
			character->mSlot[character->mActivationSlot].amount -= 1;
			//cout << client_id << "번째 유저의" << character->mActivationSlot << "번째 슬롯 아이템 1개 감소 현재 개수:" << character->mSlot[character->mActivationSlot].amount << endl;
		}
		else {
			FPP_LOG("[%d]번째 유저가 %d 번째 슬롯의 아이템이 없는데 사용하려고 시도함. - CS_PACKET_USEITEM", client_id, character->mActivationSlot.load());
		}
		
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
	case CS_PACKET_CHANGE_HOTKEYSLOT: {
		cs_packet_change_hotkeyslot* packet = reinterpret_cast<cs_packet_change_hotkeyslot*>(p);

		Character* character = reinterpret_cast<Character*>(object);
		character->mActivationSlot = packet->HotkeySlotNum;

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
		case 0: {
			if (CheatGamePlayTime)
				break;
			CheatGamePlayTime = true;

			//reset Game Play Time
			Timer_Event instq;
			instq.type = Timer_Event::TIMER_TYPE::TYPE_GAME_END;
			instq.exec_time = chrono::system_clock::now() + chrono::milliseconds(10000);
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
		}




		break;
	}
	case CS_PACKET_SYNC_BANANA: {
		cs_packet_sync_banana* packet = reinterpret_cast<cs_packet_sync_banana*>(p);
		Character* character = reinterpret_cast<Character*>(object);

		int uniqueID = character->_id * 100000000 + packet->bananaid;

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
