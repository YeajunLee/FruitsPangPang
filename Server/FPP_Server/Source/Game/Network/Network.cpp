#include <iostream>
#include <fstream>
#include <random>
#include "Network.h"
#include "../Object/Object.h"
#include "../Object/Character/Character.h"

using namespace std;
HANDLE hiocp;
SOCKET s_socket;

std::array<Object*, MAX_USER> objects;

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
		send_remove_object(OtherPlayer->_id, c_id);	//이거 언락위치 생각 
	}
}


void send_login_ok_packet(int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_login_ok packet;
	packet.id = player_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	packet.x = player->x;
	packet.y = player->y;
	player->sendPacket(&packet, sizeof(packet));
}

void send_move_packet(int player_id, int mover_id, float value, char movetype)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_move packet;
	packet.id = mover_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_MOVE;
	packet.movetype = movetype;
	packet.x = objects[mover_id]->x;
	packet.y = objects[mover_id]->y;
	packet.z = objects[mover_id]->z;
	packet.rx = objects[mover_id]->rx;
	packet.ry = objects[mover_id]->ry;
	packet.rz = objects[mover_id]->rz;
	packet.rw = objects[mover_id]->rw;
	packet.value = value;
	packet.isValid = false;
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
	float sx, float sy, float sz	//scale
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
	player->sendPacket(&packet, sizeof(packet));
}


void send_dir_packet(bool isval, int player_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_dir packet;
	packet.isValid = isval;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_DIR;
	packet.x = player->x;
	packet.y = player->y;
	packet.z = player->z;
	player->sendPacket(&packet, sizeof(packet));
}

void send_remove_object(int player_id, int removeCharacter_id)
{
	auto player = reinterpret_cast<Character*>(objects[player_id]);
	sc_packet_remove_object packet;
	packet.id = removeCharacter_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_REMOVE_OBJECT;
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
			auto character = reinterpret_cast<Character*>(other);
			character->state_lock.lock();
			if (Character::STATE::ST_INGAME == character->_state)
			{
				character->state_lock.unlock();
				send_move_packet(character->_id, client_id, packet->value, packet->movetype);
			}
			else character->state_lock.unlock();
		}
		break;
	}
	case CS_PACKET_DIR: {
		cs_packet_dir* packet = reinterpret_cast<cs_packet_dir*>(p);
		auto character = reinterpret_cast<Character*>(object);
		auto dx = abs(character->x - packet->x);
		auto dy = abs(character->y - packet->y);
		auto dz = abs(character->z - packet->z);

		//if (dy > 4)
		//{
		//	send_dir_packet(false, client_id);
		//}
		//else 
		{
			character->x = packet->x;
			character->y = packet->y;
			character->z = packet->z;
			send_dir_packet(true, client_id);
		}
		cout << dx << "," << dy << "," << dz << endl;
		break;
	}
	case CS_PACKET_ANIM: {
		cs_packet_anim* packet = reinterpret_cast<cs_packet_anim*>(p);
		cout << client_id << endl;
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
					packet->sx, packet->sy, packet->sz);
			}
			else character->state_lock.unlock();
		}
	}
	}
}
