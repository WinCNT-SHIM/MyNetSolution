#pragma once
#include "framework.h"
#include <string>
#include <list>

#define DATA_BUFSIZE 4096
using namespace std;

class Client
{
public:
	Client();
	~Client();

	SOCKET hSocket;
	string address;
	char recvBuffer[DATA_BUFSIZE];

private:
	friend ClientManager;
};

class ClientManager
{
public:
	ClientManager();
	~ClientManager();

	bool InsertNewClient(SOCKET hClientSocket, string& ip);
	bool RemoveClient(SOCKET hClientSocket);
	bool RemoveClient(SOCKET hClientSocket, string& ip);

	void OnRecv();

	int NumberOfClient() { return m_ClientList.size(); }

private:
	list<Client*> m_ClientList;

};
extern ClientManager g_ClientManager;