#include <windows.h>
#include <process.h> // _beginthreadex
#include <stdio.h>
#include "MySimpleMutex.h"

int g_Number;

HANDLE ghEvents[2];

UINT WINAPI GenNumber(LPVOID);

int main(void)
{
	HANDLE hThread[2];
	UINT i, dwEvent, dwThreadID[2];

	// 2���� �̺�Ʈ ������Ʈ ����
	for (i = 0; i < 2; i++)
	{
		ghEvents[i] = CreateEvent(
			NULL,   // default security attributes
			FALSE,  // auto-reset(True: �̺�Ʈ�� ��� �߻���Ų��)
			FALSE,  // initial state is nonsignaled
			NULL);  // unnamed object

		if (ghEvents[i] == NULL)
		{
			printf("CreateEvent error: %d\n", GetLastError());
			ExitProcess(0);
		}
	}

	// ������ ����(CreateThread�� ��� �� ��)
	for (size_t i = 0; i < sizeof(hThread) / sizeof(hThread[0]); i++)
	{
		hThread[i] = (HANDLE)_beginthreadex(
			NULL,         // ����Ʈ ���� �Ӽ�
			0,            // ����Ʈ ���� ũ��
			GenNumber,   // ������ ������ ����
			NULL,         // ������ �Լ��� �μ� ���� �� ��
			0,            // ����Ʈ ���� �÷���
			&dwThreadID[i]   // ������ ID ȹ��
		);

	}

	// ThreadProc�� ����� ������ ��ٸ��� 
	WaitForMultipleObjects(
		2,			// �迭�� ��ü �ڵ��� ����
		hThread,	// ��ü �ڵ��� �迭
		TRUE,		// TRUE: ��� ��ü�� ���°� ��ȣ�� ������ �� ��ȯ
		INFINITE		// �ð� �ʰ� ����(�и���)
	);

	// Close event handles
	for (i =
		0; i < 2; i++)
		CloseHandle(ghEvents[i]);

	return 0;
}

UINT WINAPI GenNumber(LPVOID lpParam)
{
	return 0;
}