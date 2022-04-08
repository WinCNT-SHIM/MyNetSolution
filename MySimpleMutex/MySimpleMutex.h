#pragma once
#include <windows.h>

class MySimpleMutex
{
public:
	MySimpleMutex();
	~MySimpleMutex();

	void Lock();
	void Unlock();
	void TryLock();

private:
	CRITICAL_SECTION cs;
};

