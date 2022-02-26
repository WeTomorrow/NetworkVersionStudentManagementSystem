#pragma once
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

class CSockInit
{
private:
    CSockInit();
    ~CSockInit();
    static CSockInit m_si;
};

