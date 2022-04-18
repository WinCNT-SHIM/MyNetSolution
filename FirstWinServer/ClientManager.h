#pragma once
#include "framework.h"
#include <string>
#include <list>

#define DATA_BUFSIZE 4096
using namespace std;

class Client
{
public:
	Client(SOCKET hSocket, string& ip)
	{
		this->hSocket = hSocket;
		this->address = ip;
		recvCnt = 0;
		ZeroMemory(recvBuffer, sizeof(recvBuffer));
	}
	~Client() {}

	SOCKET Handle() { return hSocket; }
	//const wchar_t* StrAddress() { return address.c_str(); }
	void Update();

	SOCKET hSocket;
	string address;
	char recvBuffer[DATA_BUFSIZE];
	int recvCnt;

private:
	//friend ClientManager;
};

class ClientManager
{
public:
	ClientManager();
	~ClientManager();

	bool InsertNewClient(SOCKET hSocket, string& ip);
	bool RemoveClient(SOCKET hSocket);
	bool RemoveClient(SOCKET hSocket, string& ip);

	Client* FindeClient(SOCKET hSocket);

	void OnRecv(SOCKET hSocket);

	int NumberOfClient() { return m_ClientList.size(); }

	//const wstring& GetLastDebugString() { return m_DbgStrBuf; }
	const wchar_t* GetLastDebugString() { return m_DbgStrBuf; }
private:
	list<Client*> m_ClientList;

	//const wstring m_dbgStr;
	wchar_t m_DbgStrBuf[DATA_BUFSIZE + 100];
};
extern ClientManager g_ClientManager;