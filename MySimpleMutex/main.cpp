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
	for (size_t i = 0; i < sizeof(hThread) / sizeof(hThread[0]); i++)
	{
		hThread[i] = (HANDLE)_beginthreadex(
			NULL,         // 디폴트 보안 속성
			0,            // 디폴트 스택 크기
			GenNumber,   // 수행할 스레드 설정
			NULL,         // 스레드 함수에 인수 설정 안 함
			0,            // 디폴트 생성 플래그
			&dwThreadID[i]   // 스레드 ID 획득
		);

	}

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

UINT WINAPI GenNumber(LPVOID lpParam)
{
	return 0;
}