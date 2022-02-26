#include "CUMT.h"
#include <time.h>
#pragma omp parallel shared

CUMT::~CUMT()
{
	Clear();
}

//服务器初始化,模仿accpet和listen
BOOL CUMT::Accept(LPCTSTR szIp, USHORT nPort)
{
	//服务器收连接
	//创建socket套接字
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		return FALSE;
	}

	//绑定端口、IP和协议族
	SOCKADDR_IN  siServer;
	siServer.sin_family = AF_INET;
	siServer.sin_addr.S_un.S_addr = inet_addr(szIp);
	siServer.sin_port = htons(nPort);
	int nRet = bind(m_sock,(sockaddr*)&siServer,sizeof(siServer));
	if (nRet == SOCKET_ERROR)
	{
		closesocket(m_sock);
		return FALSE;
	}

	while (TRUE)
	{
		//模拟三次握手的数据包
		//第一次握手,服务端接收客户端发送的PT_SYN
		CPackage pkg;
		int nLen = sizeof(m_siDst);
		nRet = recvfrom(m_sock,(char*)&pkg,sizeof(pkg),0, (sockaddr*)&m_siDst, &nLen);
		if (nRet == NULL || nRet == SOCKET_ERROR || pkg.m_nPt != PT_SYN || pkg.m_nSeq != m_nNextRecvSeq)
		{
			continue;
		}

		//第二次握手,由服务端向客户端发送带有PT_SYN | PT_ACK标志的数据包
		CPackage pkgSend(PT_SYN | PT_ACK, m_nNextSendSeq);
		nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend),0, (sockaddr*)&m_siDst, sizeof(m_siDst));
		if (nRet == SOCKET_ERROR)
		{
			continue;
		}

		//第三次握手,服务端接受客户端发来带有PT_ACK标志的数据包
		nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
		if (nRet == NULL || nRet == SOCKET_ERROR || pkg.m_nPt != PT_ACK || pkg.m_nSeq != m_nNextRecvSeq)
		{
			continue;
		}

		//连接建立后跳出循环
		break;
	}
	//建立连接初始化所有线程
	return Init();
}

BOOL CUMT::Init()
{

	//初始化
	m_nNextRecvSeq++;
	m_nNextSendSeq++;

	m_bWorking = TRUE;
	//建立发包的线程
	m_hSendThread = CreateThread(NULL, 0, SendThread, this, 0, NULL);
	if (m_hSendThread == NULL)
	{
		Clear();
		return FALSE;
	}
	//建立收包的线程
	m_hRecvThreadA = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	if (m_hRecvThreadA == NULL)
	{
		Clear();
		return FALSE;
	}
	//建立收包的线程
	m_hRecvThreadB = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	if (m_hRecvThreadB == NULL)
	{
		Clear();
		return FALSE;
	}
	//建立收包的线程
	m_hRecvThreadC = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	if (m_hRecvThreadC == NULL)
	{
		Clear();
		return FALSE;
	}

	//建立中转包容器和缓冲区的线程
	m_hHandleThread = CreateThread(NULL, 0, HandleRecvPkgsThread, this, 0, NULL);
	if (m_hHandleThread == NULL)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}

//客户端初始化，模仿connect
BOOL CUMT::Connect(LPCTSTR szIp, USHORT nPort)
{
	//客户端进行连接
	//创建socket套接字
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		return FALSE;
	}

	m_siDst.sin_family = AF_INET;
	m_siDst.sin_addr.S_un.S_addr = inet_addr(szIp);
	m_siDst.sin_port = htons(nPort);

	//模拟三次握手的数据包
	//第一次握手,客户端发送PT_SYN给服务端接收
	CPackage pkgSend(PT_SYN,m_nNextSendSeq);
	int nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst,sizeof(m_siDst));
	if (nRet == SOCKET_ERROR)
	{
		Clear();
		return FALSE;
	}
	//第二次握手,客户端接收由服务端发送带有PT_SYN | PT_ACK标志的数据包
	CPackage pkg;
	int nLen = sizeof(m_siDst);
	nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
	if (nRet == NULL || nRet == SOCKET_ERROR || pkg.m_nPt != (PT_SYN | PT_ACK) || pkg.m_nSeq != m_nNextRecvSeq)
	{
		Clear();
		return FALSE;
	}
	//第三次握手,客户端向服务端发送带有PT_ACK标志的数据包
	CPackage pkgSendAck(PT_ACK, m_nNextSendSeq);
	nRet = sendto(m_sock, (char*)&pkgSendAck, sizeof(pkgSendAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
	if (nRet == SOCKET_ERROR)
	{
		Clear();
		return FALSE;
	}
	
	//建立连接初始化所有线程
	return Init();
}

void CUMT::Log(const char* szFmt,...)
{
	char szBuff[MAXWORD]{};
	va_list vl;
	va_start(vl, szFmt);
	vsprintf(szBuff,szFmt,vl);
	va_end(vl);
	OutputDebugString(szBuff);
}


DWORD CUMT::Send(LPBYTE pBuff, DWORD dwBufLen)
{
	while (true)
	{
		m_lckForSendMap.Lock();
		DWORD dwSize = m_mapSend.size();
		m_lckForSendMap.UnLock();

		if (dwSize > 100)
		{
			Sleep(1);
			continue;
		}
		else
		{
			break;
		}

	}

	//bufWrite.Write((char*)pBuff, dwBufLen);
	m_lckForSendMap.Lock();
	DWORD dwCnt = (dwBufLen % DATALEN == 0 ? dwBufLen / DATALEN : (dwBufLen / DATALEN + 1));	//计算数据包数量

	for (DWORD i = 0; i < dwCnt; i++)
	{
		DWORD dwLen = DATALEN;
		if (i == dwCnt - 1)				//单独计算最后一个数据包的大小
		{
			dwLen = (dwBufLen - i * DATALEN);
		}
		CPackage pkg(PT_DATA, m_nNextSendSeq, pBuff + i * DATALEN, dwLen);
		m_mapSend[m_nNextSendSeq] = CPackageInfo(NULL, pkg);

		Log("[umt]: package ==> 拆包、包进容器,m_nNextSendSeq:%d", m_nNextSendSeq);
		++m_nNextSendSeq;
	}

	m_lckForSendMap.UnLock();
	return NULL;
}

DWORD CUMT::Recv(LPBYTE pBuff, DWORD dwBufLen)
{
	while (true)
	{
		m_lckForBufRecv.Lock();
		DWORD dwSize = m_bufRecv.GetSize();
		m_lckForBufRecv.UnLock();
		if (dwSize <= 0)
		{
			//没有数据,堵塞
			Sleep(1);
		}
		else
		{
			//有数据
			break;
		}
	}

	//有数据
	m_lckForBufRecv.Lock();
	DWORD dwDataLen = (m_bufRecv.GetSize() > dwBufLen ? dwBufLen : m_bufRecv.GetSize());		//读入pBuff的长度
	m_bufRecv.Read(pBuff, dwDataLen);
	m_lckForBufRecv.UnLock();

	return dwDataLen;
}

VOID CUMT::Close()
{
	Clear();
}

VOID CUMT::Clear()
{
	m_bWorking = FALSE;
	Sleep(500);
	//释放套接字
	if (m_sock != NULL)
	{
		closesocket(m_sock);
		m_sock = NULL;
	}
	//关闭发包的线程
	if (m_hSendThread != NULL)
	{
		CloseHandle(m_hSendThread);
	}
	//关闭收包的线程
	if (m_hRecvThreadA != NULL)
	{
		CloseHandle(m_hRecvThreadA);
	}
	//关闭收包的线程
	if (m_hRecvThreadB != NULL)
	{
		CloseHandle(m_hRecvThreadB);
	}
	//关闭收包的线程
	if (m_hRecvThreadC != NULL)
	{
		CloseHandle(m_hRecvThreadC);
	}
	//关闭中转包容器和缓冲区的线程
	if (m_hHandleThread != NULL)
	{
		CloseHandle(m_hHandleThread);
	}
}

DWORD CALLBACK CUMT::SendThread(LPVOID lpParam)
{
	SetThreadPriority(GetCurrentThread(), BELOW_NORMAL_PRIORITY_CLASS);	//低于标准
	CUMT* pThis = (CUMT*)lpParam;
	while (pThis->m_bWorking)
	{
		pThis->m_lckForSendMap.Lock();

		for (auto& pi:pThis->m_mapSend)
		{
			//没有发出的包,发送
			//超时的包，发送
			ULONGLONG tmCurrent = GetTickCount64();
			if (tmCurrent - pi.second.m_tmLastTime > pThis->m_tmElapse)
			{
				if (pi.second.m_tmLastTime == NULL)
				{
					pThis->Log("[umt]: package ==> 第一次发送数据包: %d",pi.second.m_pkg.m_nSeq);
				}
				else
				{
					pThis->Log("[umt]: package ==> 重发数据包: %d", pi.second.m_pkg.m_nSeq);
				}

				sendto(
					pThis->m_sock,
					(char*)&pi.second.m_pkg,sizeof(pi.second.m_pkg),
					0,
					(sockaddr*)&pThis->m_siDst,sizeof(pThis->m_siDst)
				);

				pi.second.m_tmLastTime = GetTickCount64();

			}
		}

		pThis->m_lckForSendMap.UnLock();

		//切出线程
		Sleep(1);
	}
	return NULL;
}

DWORD CALLBACK CUMT::RecvThread(LPVOID lpParam)
{
	SetThreadPriority(GetCurrentThread(), REALTIME_PRIORITY_CLASS);		//实时
	CUMT* pThis = (CUMT*)lpParam;
	while (pThis->m_bWorking)
	{

		sockaddr_in si{};
		int nLen = sizeof(si);
		CPackage pkg;
		int nRet = recvfrom(pThis->m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&si, &nLen);
		if (nRet == 0 || nRet == SOCKET_ERROR)
		{
			continue;
		}


		//判断包的类型
		switch (pkg.m_nPt)
		{
		case PT_ACK:
		{
			pThis->m_lckForSendMap.Lock();
			pThis->Log("[umt]: 接受[ack]包%d", pkg.m_nSeq);
			pThis->m_mapSend.erase(pkg.m_nSeq);			//将数据包从发送容器中踢出
			pThis->m_lckForSendMap.UnLock();
			break;
		}
		case PT_DATA:
		{
			//校验
			DWORD dwCheck = CCrc32::crc32(pkg.m_aryData, pkg.m_nLen);

			//校验失败
			if (dwCheck != pkg.m_nCheck)
			{
				//丢弃包
				break;
			}

			//校验成功，回复ACK
			CPackage pkgAck(PT_ACK, pkg.m_nSeq);
			sendto(pThis->m_sock,
				(char*)&pkgAck, sizeof(pkgAck),
				0,
				(sockaddr*)&pThis->m_siDst, sizeof(pThis->m_siDst)
			);

			//包进容器
			pThis->m_lckForRecvMap.Lock();

			if (pThis->m_mapRecv.find(pkg.m_nSeq) != pThis->m_mapRecv.end()	//容器中此序号存在(有了这个包)
				|| pkg.m_nSeq < pThis->m_nNextRecvSeq		//此序号的包中的数据已经进入缓冲区
				)
			{
				pThis->m_lckForRecvMap.UnLock();
				break;
			}
			pThis->m_mapRecv[pkg.m_nSeq] = pkg;
			pThis->Log("[umt]:将数据包存入容器");
			pThis->m_lckForRecvMap.UnLock();

			break;
		}
		default:
			break;
		}

	}
	return NULL;
}

DWORD CALLBACK CUMT::HandleRecvPkgsThread(LPVOID lpParam)
{
	CUMT* pThis = (CUMT*)lpParam;
	while (pThis->m_bWorking)
	{
		pThis->m_lckForBufRecv.Lock();
		while (true)
		{
			pThis->m_lckForRecvMap.Lock();

			if (pThis->m_mapRecv.find(pThis->m_nNextRecvSeq) != pThis->m_mapRecv.end())
			{
				//包进缓冲区
				auto& pkg = pThis->m_mapRecv[pThis->m_nNextRecvSeq];
				pThis->m_bufRecv.Write(pkg.m_aryData, pkg.m_nLen);

				pThis->Log("[umt]: 从容器存入缓冲区:%d", pThis->m_nNextRecvSeq);
				
				//从容器中移除包
				pThis->m_mapRecv.erase(pThis->m_nNextRecvSeq);

				//序号更新
				++pThis->m_nNextRecvSeq;
			}
			else
			{
				pThis->m_lckForRecvMap.UnLock();
				break;
			}
			pThis->m_lckForRecvMap.UnLock();
		}
		pThis->m_lckForBufRecv.UnLock();

		//切出线程
		Sleep(1);
	}
	return NULL;
}
