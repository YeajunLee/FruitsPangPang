#include "Tree.h"
#include <random>
#include <iostream>

using namespace std;

Tree::Tree()
	:Interact(Object::OBJTYPE::INTERACT,Interact::INTERTYPE::TREE)
	,canHarvest(true)
	,_ftype(FRUITTYPE::NONE)
	,_ttype(TREETYPE::NONE)
{

}


Tree::Tree(TREETYPE ttype)
	:Interact(Object::OBJTYPE::INTERACT, Interact::INTERTYPE::TREE)
	, canHarvest(true)
	, _ttype(ttype)
	, _ftype(FRUITTYPE::NONE)
{

	/*나중에 랜덤으로 바꿔야함*/
	if (ttype == TREETYPE::GREEN)
	{
		_ftype = FRUITTYPE::T_TOMATO;
	}
	else if (ttype == TREETYPE::ORANGE)
	{
		_ftype = FRUITTYPE::T_SUBACK;
	}
}


Tree::~Tree()
{

}


void Tree::GenerateFruit()
{
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> GreenFruit(1, 2);
	uniform_int_distribution<int> OrangeFruit(3, 3);

	if (_ttype == TREETYPE::GREEN)
	{
		_ftype = static_cast<FRUITTYPE>(GreenFruit(rng));
	}
	else if (_ttype == TREETYPE::ORANGE)
	{
		_ftype = static_cast<FRUITTYPE>(OrangeFruit(rng));
	}
	else {
		cout << "[BUG] Unvalid Tree Type! : " << static_cast<int>(_ttype) << endl;
	}

}