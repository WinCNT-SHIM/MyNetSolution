#include <windows.h>
#include <process.h> // _beginthreadex
#include <stdio.h>

HANDLE ghEvents[2];

UINT WINAPI ThreadProc(LPVOID);
UINT WINAPI GenNumber(LPVOID);
UINT WINAPI PushNumber(LPVOID);

int main(void)
{
	HANDLE hThread[2];
	UINT i, dwEvent, dwThreadID[2];
	bool isReset = false;

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
	for (size_t i = 0; i < sizeof(hThread)/sizeof(hThread[0]); i++)
	{
		hThread[i] = (HANDLE)_beginthreadex(
			NULL,         // ����Ʈ ���� �Ӽ�
			0,            // ����Ʈ ���� ũ��
			ThreadProc,   // ������ ������ ����
			NULL,         // ������ �Լ��� �μ� ���� �� ��
			0,            // ����Ʈ ���� �÷���
			&dwThreadID[i]   // ������ ID ȹ��
		);

	}

	if (hThread == NULL)
	{
		printf("CreateThread error: %d\n", GetLastError());
		return 1;
	}

	// SetEvent: ������ �̺�Ʈ ��ü�� ��ȣ ���·� �����ϴ� �Լ�
	if (!SetEvent(ghEvents[0]))
	{
		// SetEvent �Լ� ���� ���� ó��
		printf("SetEvent failed (%d)\n", GetLastError());
	}

	// Ű �Է� Ȯ��
	int key = 0;
	bool isInputKey = true; // Ű �Է��� �޴´�
	int ThreadCnt = sizeof(hThread) / sizeof(hThread[0]);
	int _EvnetSetCnt = 0;
	do 
	{
		// q �̿��� Ű�� �Է����� ���
		key = getchar();
		int temp = getchar();	// ���� ���� ����

		if (key && key == 'q')
		{
			if (!SetEvent(ghEvents[1]))
			{
				// SetEvent �Լ� ���� ���� ó��
				printf("SetEvent failed (%d)\n", GetLastError());
				return 1;
			}
			else
			{
				_EvnetSetCnt++;
				printf("Second event Set(Count: %d)\n", _EvnetSetCnt);
				if (_EvnetSetCnt == ThreadCnt) break;
			}
		}
	} while (true);


#pragma region �ٸ� ������� �����غô�
	//while (true)
	//{
	//	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	//	{
	//		if (!SetEvent(ghEvents[1]))
	//		{
	//			// SetEvent �Լ� ���� ���� ó��
	//			printf("SetEvent failed (%d)\n", GetLastError());
	//			return 1;
	//		}
	//		else
	//		{
	//			break;
	//		}
	//	}
	//}
#pragma endregion

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

UINT WINAPI ThreadProc(LPVOID lpParam)
{
	// lpParam�� ���� ��� �� ��
	UNREFERENCED_PARAMETER(lpParam);

	bool isRun = true;
	while (isRun)
	{
		// �����尡 �̺�Ʈ ��ü �� �ϳ��� ��ȣ�� ���� ������ ��ٸ���
		// ��ü �� �ϳ��� ��ȣ�� ������ ���, �� �̺�Ʈ ��ü�� ��ȯ�ϰ� ��ȣ�� Off�� �ٲ۴�
		UINT dwEvent = WaitForMultipleObjects(
			2,			// �迭�� ��ü �ڵ��� ����
			ghEvents,	// ��ü �ڵ��� �迭
			FALSE,		// FALSE: ��ü �� �ϳ��� ���°� ��ȣ�� ������ �� ��ȯ
			3000		// �ð� �ʰ� ����(�и���)
		);

		// The return value indicates which event is signaled
		switch (dwEvent)
		{
			// ghEvents[0]�� ��ȣ ������ ���
		case WAIT_OBJECT_0 + 0:
			// TODO: Perform tasks required by this event
			printf("[ThreadId: %d] First event was signaled.\n", GetCurrentThreadId());
			break;

			// ghEvents[1]�� ��ȣ ������ ���
		case WAIT_OBJECT_0 + 1:
			// TODO: ghEvents[1]�� ��ȣ ������ ���� ������ ���� ��Ŵ
			printf("[ThreadId: %d] Second event was signaled.\n", GetCurrentThreadId());
			isRun = false;
			break;
			// WaitForMultipleObjects�� Ÿ�� �ƿ�
		case WAIT_TIMEOUT:
			printf("[ThreadId: %d] Wait timed out.\n", GetCurrentThreadId());
			break;

			// WaitForMultipleObjects�� ���� ó��
		default:
			printf("Wait error: %d\n", GetLastError());
			ExitProcess(0);
		}
	}
	printf("[ThreadId: %d] Bye Bye~\n", GetCurrentThreadId());
	return 0;
}


UINT WINAPI GenNumber(LPVOID lpParam)
{
	return 0;
}