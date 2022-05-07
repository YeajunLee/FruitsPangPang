#pragma once
#include <WS2tcpip.h>

class FPPLock
{
private:
	CRITICAL_SECTION mCS;
public:
	FPPLock();
	~FPPLock();
	void lock();
	void unlock();
};

