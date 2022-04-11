#pragma once
#include <windows.h>

class MySimpleMutex
{
public:
	MySimpleMutex();
	~MySimpleMutex();

	void Lock();
	void Unlock();
	bool TryLock();

private:
	CRITICAL_SECTION cs;
};

