#include "ClientManager.h"

ClientManager g_ClientManager;

ClientManager::ClientManager()
{
}

ClientManager::~ClientManager()
{
}

bool ClientManager::InsertNewClient(SOCKET hSocket, string& ip)
{
		Client* _Client = new Client{ hSocket, ip };
		m_ClientList.push_back(_Client);
		return true;
}

bool ClientManager::RemoveClient(SOCKET hSocket)
{
	string ip;
	return RemoveClient(hSocket, ip);
}

bool ClientManager::RemoveClient(SOCKET hSocket, string& ip)
{
	for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); )
	{
		if ((*iter)->hSocket == hSocket)
		{
			ip = (*iter)->address;
			m_ClientList.erase(iter++);
			return true;
		}
	}
	return false;
}

Client* ClientManager::FindeClient(SOCKET hSocket)
{
	for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); )
	{
		if ((*iter)->hSocket == hSocket)
		{
			return (*iter);
		}
	}
	return nullptr;
}

void ClientManager::OnRecv(SOCKET hSocket)
{
	// m_ClientList에서 Client를 찾아 해당 버퍼로 recv
	Client* _thisClient = nullptr;
	_thisClient = g_ClientManager.FindeClient(hSocket);

	if (_thisClient != nullptr)
	{
		if (_thisClient->recvCnt >= DATA_BUFSIZE)
			return;

		int nRecv = recv(
			hSocket,
			_thisClient->recvBuffer + _thisClient->recvCnt,	// 수신 버퍼에 대한 포인터
			DATA_BUFSIZE,	// 버퍼의 길이(바이트)
			0);
		if (nRecv > 0)
		{
			_thisClient->recvCnt += nRecv;

			wchar_t* _tmp;
			int strSize = MultiByteToWideChar(CP_ACP, 0, _thisClient->recvBuffer, -1, NULL, NULL);
			_tmp = new WCHAR[strSize];
			MultiByteToWideChar(CP_ACP, 0, _thisClient->recvBuffer, strlen(_thisClient->recvBuffer) + 1, _tmp, strSize);

			wsprintf(m_DbgStrBuf, TEXT("Recv %s"), _tmp);
			//wsprintf(m_DbgStrBuf, TEXT("Recv %d\n"), nRecv);
			//OutputDebugStringA(_thisClient->recvBuffer);
		}
		else
		{
			// 에러 표시
			//wsprintf(buffer, TEXT("Recv %d, %d"), nRecv, WSAGetLastError());
		}
	}

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
}

void Client::Update()
{
	// recvBuffer
	// 수신 데이터를 컨트롤한다

	// 
	// 결국은 규약이 필요하다 = 프로토콜 정의
	// 텍스트 프로토콜이라면, 처리할 패킷 단위를 알려주는 시작문자와 완료 문자가 필요
	// 바이너리 프로토콜이라면, 통상은 앞의 n바이트(1바이트, 혹은 2바이트)를 패킷 사이즈
	// [패킷 사이즈][패킷 식별자] --> 패킷 헤더를 정의
	// 

}
