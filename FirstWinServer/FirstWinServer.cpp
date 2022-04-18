// FirstWinServer.cpp : TCP 소켓 연결
//

#include "framework.h"
#include "FirstWinServer.h"
#include "ClientManager.h"      // 클라이언트 관리
#include <list>
#include <string>

#define MAX_LOADSTRING 100

// ==================== 네트워크 프로그래밍 ====================
#define WM_SOCKET WM_USER + 1
BOOL InitServer(HWND hWnd);
// ==================== 네트워크 프로그래밍 ====================

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
    // ==================== 네트워크 프로그래밍 ====================
    // Winsock를 쓰겠다는 선언
	WSADATA wsd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsd))
	{
		// 윈속 시작 실패
	}
	// ==================== 네트워크 프로그래밍 ====================

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FIRSTWINSERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FIRSTWINSERVER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	// ==================== 네트워크 프로그래밍 ====================
    // Winsock 클린업
	WSACleanup();
	// ==================== 네트워크 프로그래밍 ====================
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FIRSTWINSERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FIRSTWINSERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // ==================== 네트워크 프로그래밍 ====================
   // Listen 소켓 초기화
   InitServer(hWnd);
   // ==================== 네트워크 프로그래밍 ====================

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

#pragma region ==================== 네트워크 프로그래밍 ====================
// 서버 소켓을 초기화 합니다
SOCKET Listen;
std::list<std::wstring> textList;

BOOL InitServer(HWND hWnd)
{
	SOCKADDR_IN InternetAddr;

	Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // todo: INVALID_SOCKET 확인

	InternetAddr.sin_family = AF_INET;
	/// <summary>
	/// INADDR_ANY은 알아서 잡으라는 뜻(??)
	/// </summary>
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_LOOPBACK, inet_addr("127.0.0.1");
	InternetAddr.sin_port = htons(5252);

    // 소켓에다 주소를 bind
	bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));
    // 비동기로 쓰겠다
    // WM_SOCKET는 메시지(WM_SOCKET)로 받겠다고 설정한 것
    // FD_ACCEPT | FD_CLOSE --> 이벤트, Listn용이므로 Accept만 한다
	WSAAsyncSelect(Listen, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
    // 리슨 소켓에 클라이언트들이 요청을 한다
    // 5는 backlog 지정, backlog는 클라이언트들의 요청의 대기표 같은 것
    // (커널단에서 해줌)
	listen(Listen, 5);

	textList.push_back(TEXT("Listen ... "));

	return TRUE;
}
#pragma endregion ==================== 네트워크 프로그래밍 ====================


//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
#define DATA_BUFSIZE 4096
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // ==================== 네트워크 프로그래밍 ====================
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int sockaddrLen = sizeof(clientAddr);

	char recvBuffer[DATA_BUFSIZE];
	int nRecv = 0;

	wchar_t ip[INET_ADDRSTRLEN];
	int ipLen = INET_ADDRSTRLEN;
	wchar_t buffer[1024] = TEXT("");
    // ==================== 네트워크 프로그래밍 ====================

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

			// ==================== 네트워크 프로그래밍 ====================
            // 화면에 텍스트를 그리기 위한 것
			if (textList.size() > 20)
			{
				textList.pop_front();
			}

			int line = 0;
			for (auto text : textList)
			{
				TextOut(hdc, 1, ++line * 20, text.c_str(), text.length());
			}
			// ==================== 네트워크 프로그래밍 ====================

            EndPaint(hWnd, &ps);
        }
        break;
    // ==================== 네트워크 프로그래밍 ====================
	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lParam))
		{
			closesocket((SOCKET)wParam);
			break;
		}

		switch (WSAGETSELECTEVENT(lParam))
		{

		case FD_ACCEPT:
        {
			clientSocket = accept(wParam, (SOCKADDR*)&clientAddr, &sockaddrLen);
            // FD_READ | FD_WRITE | FD_CLOSE의 시그널이 오면
            // WM_SOCKET으로 알려달라고 OS에게 부탁하는 것
			WSAAsyncSelect(clientSocket, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);

			WSAAddressToString((struct sockaddr*)&clientAddr, sizeof(struct sockaddr_in), 0, ip, (LPDWORD)&ipLen);

			wstring ws(ip);
			string ip_string(ws.begin(), ws.end());
			g_ClientManager.InsertNewClient(clientSocket, ip_string);

			wsprintf(buffer, TEXT("Client Accepted %s(%d)"), ip, g_ClientManager.NumberOfClient());

			textList.push_back(buffer);
			InvalidateRect(hWnd, NULL, TRUE);


		}
        break;

		case FD_READ:
            g_ClientManager.OnRecv((SOCKET) wParam);
            textList.push_back(g_ClientManager.GetLastDebugString());
            /*
            // recv: 리시브
            // wParam를 리시브 버퍼에 쓴다
			nRecv = recv((SOCKET)wParam, recvBuffer, DATA_BUFSIZE, 0);
			if (nRecv > 0)
			{
				wsprintf(buffer, TEXT("Recv %d"), nRecv);
			}
			else
			{
                // 에러 표시
				wsprintf(buffer, TEXT("Recv %d, %d"), nRecv, WSAGetLastError());
			}

			textList.push_back(buffer);
			*/

			InvalidateRect(hWnd, NULL, TRUE);

			break;

		case FD_WRITE:
			// The socket in wParam is ready for sending data
			break;

		case FD_CLOSE:
		{
            string _ClientIP;
			g_ClientManager.RemoveClient((SOCKET)wParam, _ClientIP);
			closesocket((SOCKET)wParam);

			wstring _ClientIP_w;
			_ClientIP_w.assign(_ClientIP.begin(), _ClientIP.end());

			// 어떤 클라이언트가 종료되었는지 찍어보자
			// 
			wsprintf(buffer, TEXT("Client Closed %s(%d)"), _ClientIP_w.c_str(), g_ClientManager.NumberOfClient());

			textList.push_back(buffer);

			InvalidateRect(hWnd, NULL, TRUE);
        }
		break;

		}
		break;
	}
	// ==================== 네트워크 프로그래밍 ====================
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
