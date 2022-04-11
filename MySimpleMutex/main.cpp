#include <windows.h>
#include <process.h> // _beginthreadex
#include <stdio.h>
#include <iostream>
#include <queue>

#include "MySimpleMutex.h"

using namespace std;

// ���
const unsigned int THREAD_CNT = 2;

// ���� ����
unsigned int g_Number = 0;
MySimpleMutex g_Mutex;
queue<unsigned int> g_Queue;
queue<unsigned int> g_Queue_ThreadID;

// �Լ� ���� ����
UINT WINAPI GenNumber(LPVOID);
UINT WINAPI PushNumber_NonSync(LPVOID);
UINT WINAPI PushNumber_Interlocked(LPVOID);
UINT WINAPI PushNumber(LPVOID);

int main(void)
{
	HANDLE hThread[THREAD_CNT];
	UINT i, dwEvent, dwThreadID[2];
	g_Mutex = MySimpleMutex();

	// ������ ����(CreateThread�� ��� �� ��)
	for (size_t i = 0; i < THREAD_CNT; i++)
	{
		hThread[i] = (HANDLE)_beginthreadex(
			NULL,			// ����Ʈ ���� �Ӽ�
			0,				// ����Ʈ ���� ũ��
			//GenNumber,
			//PushNumber_NonSync,
			PushNumber_Interlocked,
			//PushNumber,
			NULL,			// ������ �Լ��� �μ� ���� �� ��
			0,				// ����Ʈ ���� �÷���
			&dwThreadID[i]	// ������ ID ȹ��
		);

	}

	// ThreadProc�� ����� ������ ��ٸ��� 
	WaitForMultipleObjects(
		THREAD_CNT,	// �迭�� ��ü �ڵ��� ����
		hThread,	// ��ü �ڵ��� �迭
		TRUE,		// TRUE: ��� ��ü�� ���°� ��ȣ�� ������ �� ��ȯ
		INFINITE	// �ð� �ʰ� ����(�и���)
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
			// ť�� ����ȭ
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