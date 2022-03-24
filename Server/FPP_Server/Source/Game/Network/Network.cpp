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
concurrency::concurrent_priority_queue <Timer_Event> timer_queue;

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

void send_throwfruit_packet(int thrower_character_id, int other_character_id,
	float rx, float ry, float rz, float rw,	//rotate
	float lx, float ly, float lz,	//location
	float sx, float sy, float sz,	//scale
	int fruittype	//item code
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
		send_login_ok_packet(client_id);

		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			if (other->_id == client_id) continue;


			auto OtherPlayer = reinterpret_cast<Character*>(other);
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
			OtherPlayer->sendPacket(&packet, sizeof(packet));
		}

		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			if (other->_id == client_id) continue;
			auto OtherPlayer = reinterpret_cast<Character*>(other);
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

			character->sendPacket(&packet, sizeof(packet));
		}
		break;
	}
	case CS_PACKET_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p);
		//switch (packet->direction) {
		//case 0: if (y > 0) y--; break;
		//case 1: if (y < (WORLD_HEIGHT - 1)) y++; break;
		//case 2: if (x > 0) x--; break;
		//case 3: if (x < (WORLD_WIDTH - 1)) x++; break;
		//default:
		//	std::cout << "Invalid move in client " << client_id << endl;
		//	exit(-1);
		//}
		object->x = packet->x;
		object->y = packet->y;
		object->z = packet->z;
		object->rx = packet->rx;
		object->ry = packet->ry;
		object->rz = packet->rz;
		object->rw = packet->rw;

		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			auto character = reinterpret_cast<Character*>(other);
			character->state_lock.lock();
			if (Character::STATE::ST_INGAME == character->_state)
			{
				character->state_lock.unlock();
				send_move_packet(character->_id, client_id, packet->speed);
			}
			else character->state_lock.unlock();
		}
		break;
	}
	case CS_PACKET_ANIM: {
		cs_packet_anim* packet = reinterpret_cast<cs_packet_anim*>(p);
		cout << client_id << endl;

		switch (packet->animtype)
		{
		case 0://Throw
		{
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
		}
		

		break;
	}
	case CS_PACKET_SPAWNOBJ: {
		cs_packet_spawnobj* packet = reinterpret_cast<cs_packet_spawnobj*>(p);

		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			if (other->_id == client_id) continue;
			auto character = reinterpret_cast<Character*>(other);
			character->state_lock.lock();
			if (Character::STATE::ST_INGAME == character->_state)
			{
				character->state_lock.unlock();
				send_throwfruit_packet(client_id, character->_id,
					packet->rx, packet->ry, packet->rz, packet->rw,
					packet->lx, packet->ly, packet->lz,
					packet->sx, packet->sy, packet->sz,
					packet->fruitType);
			}
			else character->state_lock.unlock();
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
		
		cout << "과일 받았습니다(나무)" << endl;
		switch (tree->_ttype)
		{
		case TREETYPE::GREEN:
			character->UpdateInventorySlotAtIndex(0, tree->_ftype, 5);
			send_update_inventory_packet(client_id, 0);
			break;
		case TREETYPE::ORANGE:
			character->UpdateInventorySlotAtIndex(1, tree->_ftype, 5);
			send_update_inventory_packet(client_id, 1);
			break;
		}
		//character->mSlot[0].type = tree->_ftype;
		//character->mSlot[0].amount++;
		//send_update_inventory(client_id, 0);
		tree->interact();
		//Timer_Event instq;
		//instq.exec_time = chrono::system_clock::now() + 5000ms;
		//instq.type = Timer_Event::TIMER_TYPE::TYPE_TREE_RESPAWN;
		//instq.object_id = packet->tree_id;
		//timer_queue.push(instq);

		for (auto& other : objects)
		{
			if (!other->isPlayer()) break;
			auto player = reinterpret_cast<Character*>(other);
			if (player->_state == Character::STATE::ST_INGAME)
			{
				cout << "과일나무 떨어졌다고 보냅니다"<<packet->obj_id<<"," << endl;
				send_update_interstat_packet(other->_id, packet->obj_id, false, INTERACT_TYPE_TREE);
			}
		}
		tree->canHarvest = false;
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

		cout << "과일 받았습니다(과일상자)" << endl;
		if (punnet->_ftype == FRUITTYPE::T_HEAL)
		{
			character->Heal(10);
		}
		else {
			character->UpdateInventorySlotAtIndex(3, punnet->_ftype, 5);
			send_update_inventory_packet(client_id, 3);
		}

		
		punnet->interact();

		for (auto& other : objects)
		{
			if (!other->isPlayer()) break;
			auto player = reinterpret_cast<Character*>(other);
			if (player->_state == Character::STATE::ST_INGAME)
			{
				cout << "과일박스 먹었다고 보냅니다" <<packet->obj_id<<"," << endl;
				send_update_interstat_packet(other->_id, packet->obj_id, false, INTERACT_TYPE_PUNNET);
			}
		}
		punnet->canHarvest = false;
		break;
	}
	case CS_PACKET_USEITEM: {

		cs_packet_useitem* packet = reinterpret_cast<cs_packet_useitem*>(p);
		Character* character = reinterpret_cast<Character*>(object);
		//character->mSlot[packet->slotNum].amount -= packet->Amount;
		character->mSlot[character->mActivationSlot].amount -= 1;
		cout << client_id<<"번째 유저의" << character->mActivationSlot << "번째 슬롯 아이템 1개 감소 현재 개수:" << character->mSlot[character->mActivationSlot].amount << endl;
		break;
	}
	case CS_PACKET_HIT: {
		cs_packet_hit* packet = reinterpret_cast<cs_packet_hit*>(p);
		Character* character = reinterpret_cast<Character*>(object);
		cout << client_id << "의 이전 hp : " << character->hp << endl;
		character->Hurt(10);
		cout << client_id << "의 이후 hp : " << character->hp << endl;
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
			instq.object_id = static_cast<int>(packet->x);
			instq.player_id = static_cast<int>(packet->y);
			instq.spare = static_cast<int>(packet->z);
			instq.spare2 = 5;
			instq.type = Timer_Event::TIMER_TYPE::TYPE_DURIAN_DMG;
			instq.exec_time = chrono::system_clock::now() + 2000ms;
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
		RespawnPlayer->hp = RespawnPlayer->maxhp;
		RespawnPlayer->rx = 0, RespawnPlayer->ry = 0, RespawnPlayer->rz = 0, RespawnPlayer->rw = 1;
		switch (packet->numbering)
		{
		case 0:
			RespawnPlayer->x = 18090, RespawnPlayer->y = 17480, RespawnPlayer->z = 100;
			break;
		case 1:
			RespawnPlayer->x = 9880, RespawnPlayer->y = 18390, RespawnPlayer->z = 100;
			break;
		case 2:
			RespawnPlayer->x = 2200, RespawnPlayer->y = 18140, RespawnPlayer->z = 100;
			break;
		case 3:
			RespawnPlayer->x = 18350, RespawnPlayer->y = 9610, RespawnPlayer->z = 100;
			break;
		case 4:
			RespawnPlayer->x =1750, RespawnPlayer->y =10220, RespawnPlayer->z = 100;
			break;
		case 5:
			RespawnPlayer->x =17980, RespawnPlayer->y = 1780, RespawnPlayer->z = 100;
			break;
		case 6:
			RespawnPlayer->x = 9730, RespawnPlayer->y = 1620, RespawnPlayer->z = 100;
			break;
		case 7:
			RespawnPlayer->x = 1990, RespawnPlayer->y = 1740, RespawnPlayer->z = 100;
			break;
		case 8:
			RespawnPlayer->x = 32030, RespawnPlayer->y = 24480, RespawnPlayer->z = 398;
			break;
		}

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
	}
}
