#include "Punnet.h"
#include "../../../Network/Network.h"

using namespace std;



Punnet::Punnet()
	:Interaction(Object::OBJTYPE::INTERACT, Interaction::INTERTYPE::PUNNET)
	, canHarvest(true)
	,_ftype(FRUITTYPE::T_NUT)
{

}

Punnet::~Punnet()
{

}


void Punnet::interact()
{
	Interaction::interact();

	//Timer_Event instq;
	//instq.exec_time = chrono::system_clock::now() + 5000ms;
	//instq.type = Timer_Event::TIMER_TYPE::TYPE_TREE_RESPAWN;
	//instq.object_id = _id;
	//timer_queue.push(instq);
}