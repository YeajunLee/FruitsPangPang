#include "Banana.h"
#include "../../Character/Character.h"
Banana::Banana()
	:Interaction(Object::OBJTYPE::INTERACT, Interaction::INTERTYPE::SPECIAL)
	, _ftype(FRUITTYPE::T_BANANA)
	, _state(Banana::STATE::ST_FREE)
{

}

Banana::~Banana()
{
}

void Banana::ResetObject()
{
	Interaction::ResetObject();
}

void Banana::GenerateFruit()
{
	//바나나 spawn 패킷. id를 담아서 보냄. 
}

void Banana::interact(Object* interactobj)
{
	if (interactobj->_type != Object::OBJTYPE::PLAYER) return;

	auto player = reinterpret_cast<Character*>(interactobj);
	bool isStepped = false;

	state_lock.lock();
	if (_state == Banana::STATE::ST_ACTIVE)
	{
		isStepped = true;
		_state = Banana::STATE::ST_JUDGE;
		state_lock.unlock();
	}
	else {
		state_lock.unlock();
		//먼저 밟은 사람이 있다면 아래 구문 필요없음. return.
		return;
	}


	if (isStepped)
	{
		for (auto& other : objects) {
			if (!other->isPlayer()) break;
			auto OtherPlayer = reinterpret_cast<Character*>(other);
			if (player->bAi && OtherPlayer->bAi) continue;
			OtherPlayer->state_lock.lock();
			if (Character::STATE::ST_INGAME == OtherPlayer->_state)
			{
				OtherPlayer->state_lock.unlock();
				send_step_banana_packet(OtherPlayer->_id, player->_id, this->_id);
			}
			else OtherPlayer->state_lock.unlock();
		}
		//밟는 interact
		// 클라에서는 이 packet을 받아 Anim, Destory를 전부 해준다. 
	}
	
	
	//판정 끝. FREE상태
	state_lock.lock();
	_state = Banana::STATE::ST_FREE;
	state_lock.unlock();
}
