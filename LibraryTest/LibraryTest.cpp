#pragma once
#define WIN32_LEAN_AND_MEAN	// ����� �浹�� ��������
#include <windows.h>
#include <iostream>

// ��ο� ���Ӽ� üũ
#include "..\\MyFirstNetwork\\WinNetwork.h"
#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\MyFirstNetwork.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\MyFirstNetwork.lib")
#endif


int main()
{

}
