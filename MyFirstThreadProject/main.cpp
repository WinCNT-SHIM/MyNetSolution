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

	// 2개의 이벤트 오브젝트 생성
	for (i = 0; i < 2; i++)
	{
		ghEvents[i] = CreateEvent(
			NULL,   // default security attributes
			FALSE,  // auto-reset(True: 이벤트를 계속 발생시킨다)
			FALSE,  // initial state is nonsignaled
			NULL);  // unnamed object

		if (ghEvents[i] == NULL)
		{
			printf("CreateEvent error: %d\n", GetLastError());
			ExitProcess(0);
		}
	}

	// 스레드 생성(CreateThread는 사용 안 함)
	for (size_t i = 0; i < sizeof(hThread)/sizeof(hThread[0]); i++)
	{
		hThread[i] = (HANDLE)_beginthreadex(
			NULL,         // 디폴트 보안 속성
			0,            // 디폴트 스택 크기
			ThreadProc,   // 수행할 스레드 설정
			NULL,         // 스레드 함수에 인수 설정 안 함
			0,            // 디폴트 생성 플래그
			&dwThreadID[i]   // 스레드 ID 획득
		);

	}

	if (hThread == NULL)
	{
		printf("CreateThread error: %d\n", GetLastError());
		return 1;
	}

	// SetEvent: 지정된 이벤트 객체를 신호 상태로 설정하는 함수
	if (!SetEvent(ghEvents[0]))
	{
		// SetEvent 함수 실패 시의 처리
		printf("SetEvent failed (%d)\n", GetLastError());
	}

	// 키 입력 확인
	int key = 0;
	bool isInputKey = true; // 키 입력을 받는다
	int ThreadCnt = sizeof(hThread) / sizeof(hThread[0]);
	int _EvnetSetCnt = 0;
	do 
	{
		// q 이외의 키를 입력했을 경우
		key = getchar();
		int temp = getchar();	// 엔터 값은 버림

		if (key && key == 'q')
		{
			if (!SetEvent(ghEvents[1]))
			{
				// SetEvent 함수 실패 시의 처리
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


#pragma region 다른 방식으로 구현해봤다
	//while (true)
	//{
	//	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	//	{
	//		if (!SetEvent(ghEvents[1]))
	//		{
	//			// SetEvent 함수 실패 시의 처리
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

	// ThreadProc가 종료될 때까지 기다린다 
	WaitForMultipleObjects(
		2,			// 배열의 개체 핸들의 개수
		hThread,	// 개체 핸들의 배열
		TRUE,		// TRUE: 모든 객체의 상태가 신호로 설정될 때 반환
		INFINITE		// 시간 초과 간격(밀리초)
	);

	// Close event handles
	for (i = 
		0; i < 2; i++)
		CloseHandle(ghEvents[i]);

	return 0;
}

UINT WINAPI ThreadProc(LPVOID lpParam)
{
	// lpParam의 값은 사용 안 함
	UNREFERENCED_PARAMETER(lpParam);

	bool isRun = true;
	while (isRun)
	{
		// 스레드가 이벤트 개체 중 하나에 신호를 보낼 때까지 기다린다
		// 객체 중 하나가 신호를 보냈을 경우, 그 이벤트 객체를 반환하고 신호를 Off로 바꾼다
		UINT dwEvent = WaitForMultipleObjects(
			2,			// 배열의 개체 핸들의 개수
			ghEvents,	// 개체 핸들의 배열
			FALSE,		// FALSE: 객체 중 하나의 상태가 신호로 설정될 때 반환
			3000		// 시간 초과 간격(밀리초)
		);

		// The return value indicates which event is signaled
		switch (dwEvent)
		{
			// ghEvents[0]가 신호 상태일 경우
		case WAIT_OBJECT_0 + 0:
			// TODO: Perform tasks required by this event
			printf("[ThreadId: %d] First event was signaled.\n", GetCurrentThreadId());
			break;

			// ghEvents[1]가 신호 상태일 경우
		case WAIT_OBJECT_0 + 1:
			// TODO: ghEvents[1]가 신호 상태일 경우는 스레드 종료 시킴
			printf("[ThreadId: %d] Second event was signaled.\n", GetCurrentThreadId());
			isRun = false;
			break;
			// WaitForMultipleObjects의 타임 아웃
		case WAIT_TIMEOUT:
			printf("[ThreadId: %d] Wait timed out.\n", GetCurrentThreadId());
			break;

			// WaitForMultipleObjects의 예외 처리
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