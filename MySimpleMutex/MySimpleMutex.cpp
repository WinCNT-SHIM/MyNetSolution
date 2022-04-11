#include "MySimpleMutex.h"

MySimpleMutex::MySimpleMutex()
{
	InitializeCriticalSection(&cs);
}

MySimpleMutex::~MySimpleMutex()
{
	DeleteCriticalSection(&cs);
}

void MySimpleMutex::Lock()
{
	EnterCriticalSection(&cs);
}

void MySimpleMutex::Unlock()
{
	LeaveCriticalSection(&cs);
}

bool MySimpleMutex::TryLock()
{
	return TryEnterCriticalSection(&cs);
}
