#include <windows.h>
#include <process.h> // _beginthreadex
#include <stdio.h>
#include <iostream>
#include <queue>

#include "MySimpleMutex.h"

using namespace std;

// 상수
const unsigned int THREAD_CNT = 2;

// 전역 변수
unsigned int g_Number = 0;
MySimpleMutex g_Mutex;
queue<unsigned int> g_Queue;
queue<unsigned int> g_Queue_ThreadID;

// 함수 전방 선언
UINT WINAPI GenNumber(LPVOID);
UINT WINAPI PushNumber_NonSync(LPVOID);
UINT WINAPI PushNumber_Interlocked(LPVOID);
UINT WINAPI PushNumber(LPVOID);

int main(void)
{
	HANDLE hThread[THREAD_CNT];
	UINT i, dwEvent, dwThreadID[2];
	g_Mutex = MySimpleMutex();

	// 스레드 생성(CreateThread는 사용 안 함)
	for (size_t i = 0; i < THREAD_CNT; i++)
	{
		hThread[i] = (HANDLE)_beginthreadex(
			NULL,			// 디폴트 보안 속성
			0,				// 디폴트 스택 크기
			//GenNumber,
			//PushNumber_NonSync,
			PushNumber_Interlocked,
			//PushNumber,
			NULL,			// 스레드 함수에 인수 설정 안 함
			0,				// 디폴트 생성 플래그
			&dwThreadID[i]	// 스레드 ID 획득
		);

	}

	// ThreadProc가 종료될 때까지 기다린다 
	WaitForMultipleObjects(
		THREAD_CNT,	// 배열의 개체 핸들의 개수
		hThread,	// 개체 핸들의 배열
		TRUE,		// TRUE: 모든 객체의 상태가 신호로 설정될 때 반환
		INFINITE	// 시간 초과 간격(밀리초)
	);

	//int _cnt = 0;
	//while (g_Queue.size() != 0)
	for (int _cnt = 0; g_Queue.size() != 0; _cnt++)
	{
		if (_cnt % 10 == 0 && _cnt != 0)
		{
			cout << endl;
		}
		cout << g_Queue.front();
		g_Queue.pop();
		if (_cnt != g_Queue.size() - 1)
		{
			cout << ", ";
		}
	}
	cout << endl;
	cout << "Result: " << g_Number;

	return 0;
}

UINT WINAPI GenNumber(LPVOID lpParam)
{
	while (true)
	{
		if (g_Number < 10000)
		{
			g_Mutex.Lock();
			if (g_Number < 10000)
				++g_Number;
			g_Mutex.Unlock();
		}
		else break;
	}
	return 0;
}

UINT WINAPI PushNumber_NonSync(LPVOID lpParam)
{
	while (true)
	{
		if (g_Number < 10000)
		{
			++g_Number;
			// 큐만 동기화
			g_Mutex.Lock();
			g_Queue.push(g_Number);
			g_Mutex.Unlock();
		}
		else break;
	}
	return 0;
}

UINT WINAPI PushNumber_Interlocked(LPVOID lpParam)
{
	for (int i = 0; i < 10000; i++)
	{
		if (g_Number < 10000)
		{
			unsigned res = InterlockedIncrement(&g_Number);
			g_Mutex.Lock();
			g_Queue_ThreadID.push((unsigned int)GetCurrentThreadId());
			g_Queue.push(res);
			g_Mutex.Unlock();
		}
		else
		{
			break;
		}
	}
	return 0;
}

UINT WINAPI PushNumber(LPVOID lpParam)
{
	while (true)
	{
		if (g_Number < 10000)
		{
			g_Mutex.Lock();
			if (g_Number < 10000)
			{
				++g_Number;
				g_Queue.push(g_Number);
			}
			g_Mutex.Unlock();
		}
		else
		{
			break;
		}
	}
	return 0;
}