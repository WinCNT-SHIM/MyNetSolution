#include "ClientManager.h"

ClientManager g_ClientManager;

ClientManager::ClientManager()
{
}

ClientManager::~ClientManager()
{
}

bool ClientManager::InsertNewClient(SOCKET hClientSocket, string& ip)
{
		Client* _Client = new Client{ hClientSocket, ip };
		m_ClientList.push_back(_Client);
		return true;
}

bool ClientManager::RemoveClient(SOCKET hClientSocket)
{
	string ip;
	return RemoveClient(hClientSocket, ip);
}

bool ClientManager::RemoveClient(SOCKET hClientSocket, string& ip)
{
	for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); )
	{
		if ((*iter)->hSocket == hClientSocket)
		{
			ip = (*iter)->address;
			m_ClientList.erase(iter++);
			return true;
		}
	}
	return false;
}
