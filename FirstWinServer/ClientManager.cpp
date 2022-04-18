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
	// m_ClientList���� Client�� ã�� �ش� ���۷� recv
	Client* _thisClient = nullptr;
	_thisClient = g_ClientManager.FindeClient(hSocket);

	if (_thisClient != nullptr)
	{
		if (_thisClient->recvCnt >= DATA_BUFSIZE)
			return;

		int nRecv = recv(
			hSocket,
			_thisClient->recvBuffer + _thisClient->recvCnt,	// ���� ���ۿ� ���� ������
			DATA_BUFSIZE,	// ������ ����(����Ʈ)
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
			// ���� ǥ��
			//wsprintf(buffer, TEXT("Recv %d, %d"), nRecv, WSAGetLastError());
		}
	}

	/*
			// recv: ���ú�
			// wParam�� ���ú� ���ۿ� ����
			nRecv = recv((SOCKET)wParam, recvBuffer, DATA_BUFSIZE, 0);
			if (nRecv > 0)
			{
				wsprintf(buffer, TEXT("Recv %d"), nRecv);
			}
			else
			{
				// ���� ǥ��
				wsprintf(buffer, TEXT("Recv %d, %d"), nRecv, WSAGetLastError());
			}

			textList.push_back(buffer);
	*/
}

void Client::Update()
{
	// recvBuffer
	// ���� �����͸� ��Ʈ���Ѵ�

	// 
	// �ᱹ�� �Ծ��� �ʿ��ϴ� = �������� ����
	// �ؽ�Ʈ ���������̶��, ó���� ��Ŷ ������ �˷��ִ� ���۹��ڿ� �Ϸ� ���ڰ� �ʿ�
	// ���̳ʸ� ���������̶��, ����� ���� n����Ʈ(1����Ʈ, Ȥ�� 2����Ʈ)�� ��Ŷ ������
	// [��Ŷ ������][��Ŷ �ĺ���] --> ��Ŷ ����� ����
	// 

}
