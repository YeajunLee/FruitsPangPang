#pragma once
#include <mutex>
#include "../Object.h"


class Interaction : public Object
{
public:
	enum class INTERTYPE { NONE, TREE, PUNNET, HEAL };	//interact_type [tree,punnet]

	Interaction(OBJTYPE type = OBJTYPE::INTERACT, INTERTYPE itype = INTERTYPE::NONE);
	virtual ~Interaction();
	virtual void ResetObject() override;
	virtual void interact(class Object* interactobj);
	virtual void GenerateFruit();
	bool CanInteract;
	std::mutex CanInteract_lock;
	INTERTYPE _itype;

};

