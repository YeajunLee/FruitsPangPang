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
	//�ٳ��� spawn ��Ŷ. id�� ��Ƽ� ����. 
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
		//���� ���� ����� �ִٸ� �Ʒ� ���� �ʿ����. return.
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
		//��� interact
		// Ŭ�󿡼��� �� packet�� �޾� Anim, Destory�� ���� ���ش�. 
	}
	
	
	//���� ��. FREE����
	state_lock.lock();
	_state = Banana::STATE::ST_FREE;
	state_lock.unlock();
}
