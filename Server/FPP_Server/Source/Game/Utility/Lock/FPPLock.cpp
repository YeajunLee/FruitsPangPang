#include "FPPLock.h"
#include <iostream>

FPPLock::FPPLock()
{
	InitializeCriticalSection(&mCS);
}

FPPLock::~FPPLock()
{
	DeleteCriticalSection(&mCS);
}

void FPPLock::lock()
{
	EnterCriticalSection(&mCS);
}

void FPPLock::unlock()
{
	LeaveCriticalSection(&mCS);
}
