#pragma once
#define WIN32_LEAN_AND_MEAN	// 헤더의 충돌을 방지해줌
#include <windows.h>
#include <iostream>

// 경로와 종속성 체크
#include "..\\MyFirstNetwork\\WinNetwork.h"
#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\MyFirstNetwork.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\MyFirstNetwork.lib")
#endif


int main()
{

}
