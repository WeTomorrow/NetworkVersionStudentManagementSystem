#include "CUMT.h"
#include <time.h>
#pragma omp parallel shared

CUMT::~CUMT()
{
	Clear();
}

//��������ʼ��,ģ��accpet��listen
BOOL CUMT::Accept(LPCTSTR szIp, USHORT nPort)
{
	//������������
	//����socket�׽���
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		return FALSE;
	}

	//�󶨶˿ڡ�IP��Э����
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
		//ģ���������ֵ����ݰ�
		//��һ������,����˽��տͻ��˷��͵�PT_SYN
		CPackage pkg;
		int nLen = sizeof(m_siDst);
		nRet = recvfrom(m_sock,(char*)&pkg,sizeof(pkg),0, (sockaddr*)&m_siDst, &nLen);
		if (nRet == NULL || nRet == SOCKET_ERROR || pkg.m_nPt != PT_SYN || pkg.m_nSeq != m_nNextRecvSeq)
		{
			continue;
		}

		//�ڶ�������,�ɷ������ͻ��˷��ʹ���PT_SYN | PT_ACK��־�����ݰ�
		CPackage pkgSend(PT_SYN | PT_ACK, m_nNextSendSeq);
		nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend),0, (sockaddr*)&m_siDst, sizeof(m_siDst));
		if (nRet == SOCKET_ERROR)
		{
			continue;
		}

		//����������,����˽��ܿͻ��˷�������PT_ACK��־�����ݰ�
		nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
		if (nRet == NULL || nRet == SOCKET_ERROR || pkg.m_nPt != PT_ACK || pkg.m_nSeq != m_nNextRecvSeq)
		{
			continue;
		}

		//���ӽ���������ѭ��
		break;
	}
	//�������ӳ�ʼ�������߳�
	return Init();
}

BOOL CUMT::Init()
{

	//��ʼ��
	m_nNextRecvSeq++;
	m_nNextSendSeq++;

	m_bWorking = TRUE;
	//�����������߳�
	m_hSendThread = CreateThread(NULL, 0, SendThread, this, 0, NULL);
	if (m_hSendThread == NULL)
	{
		Clear();
		return FALSE;
	}
	//�����հ����߳�
	m_hRecvThreadA = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	if (m_hRecvThreadA == NULL)
	{
		Clear();
		return FALSE;
	}
	//�����հ����߳�
	m_hRecvThreadB = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	if (m_hRecvThreadB == NULL)
	{
		Clear();
		return FALSE;
	}
	//�����հ����߳�
	m_hRecvThreadC = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	if (m_hRecvThreadC == NULL)
	{
		Clear();
		return FALSE;
	}

	//������ת�������ͻ��������߳�
	m_hHandleThread = CreateThread(NULL, 0, HandleRecvPkgsThread, this, 0, NULL);
	if (m_hHandleThread == NULL)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}

//�ͻ��˳�ʼ����ģ��connect
BOOL CUMT::Connect(LPCTSTR szIp, USHORT nPort)
{
	//�ͻ��˽�������
	//����socket�׽���
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		return FALSE;
	}

	m_siDst.sin_family = AF_INET;
	m_siDst.sin_addr.S_un.S_addr = inet_addr(szIp);
	m_siDst.sin_port = htons(nPort);

	//ģ���������ֵ����ݰ�
	//��һ������,�ͻ��˷���PT_SYN������˽���
	CPackage pkgSend(PT_SYN,m_nNextSendSeq);
	int nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst,sizeof(m_siDst));
	if (nRet == SOCKET_ERROR)
	{
		Clear();
		return FALSE;
	}
	//�ڶ�������,�ͻ��˽����ɷ���˷��ʹ���PT_SYN | PT_ACK��־�����ݰ�
	CPackage pkg;
	int nLen = sizeof(m_siDst);
	nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
	if (nRet == NULL || nRet == SOCKET_ERROR || pkg.m_nPt != (PT_SYN | PT_ACK) || pkg.m_nSeq != m_nNextRecvSeq)
	{
		Clear();
		return FALSE;
	}
	//����������,�ͻ��������˷��ʹ���PT_ACK��־�����ݰ�
	CPackage pkgSendAck(PT_ACK, m_nNextSendSeq);
	nRet = sendto(m_sock, (char*)&pkgSendAck, sizeof(pkgSendAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
	if (nRet == SOCKET_ERROR)
	{
		Clear();
		return FALSE;
	}
	
	//�������ӳ�ʼ�������߳�
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
	DWORD dwCnt = (dwBufLen % DATALEN == 0 ? dwBufLen / DATALEN : (dwBufLen / DATALEN + 1));	//�������ݰ�����

	for (DWORD i = 0; i < dwCnt; i++)
	{
		DWORD dwLen = DATALEN;
		if (i == dwCnt - 1)				//�����������һ�����ݰ��Ĵ�С
		{
			dwLen = (dwBufLen - i * DATALEN);
		}
		CPackage pkg(PT_DATA, m_nNextSendSeq, pBuff + i * DATALEN, dwLen);
		m_mapSend[m_nNextSendSeq] = CPackageInfo(NULL, pkg);

		Log("[umt]: package ==> �������������,m_nNextSendSeq:%d", m_nNextSendSeq);
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
			//û������,����
			Sleep(1);
		}
		else
		{
			//������
			break;
		}
	}

	//������
	m_lckForBufRecv.Lock();
	DWORD dwDataLen = (m_bufRecv.GetSize() > dwBufLen ? dwBufLen : m_bufRecv.GetSize());		//����pBuff�ĳ���
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
	//�ͷ��׽���
	if (m_sock != NULL)
	{
		closesocket(m_sock);
		m_sock = NULL;
	}
	//�رշ������߳�
	if (m_hSendThread != NULL)
	{
		CloseHandle(m_hSendThread);
	}
	//�ر��հ����߳�
	if (m_hRecvThreadA != NULL)
	{
		CloseHandle(m_hRecvThreadA);
	}
	//�ر��հ����߳�
	if (m_hRecvThreadB != NULL)
	{
		CloseHandle(m_hRecvThreadB);
	}
	//�ر��հ����߳�
	if (m_hRecvThreadC != NULL)
	{
		CloseHandle(m_hRecvThreadC);
	}
	//�ر���ת�������ͻ��������߳�
	if (m_hHandleThread != NULL)
	{
		CloseHandle(m_hHandleThread);
	}
}

DWORD CALLBACK CUMT::SendThread(LPVOID lpParam)
{
	SetThreadPriority(GetCurrentThread(), BELOW_NORMAL_PRIORITY_CLASS);	//���ڱ�׼
	CUMT* pThis = (CUMT*)lpParam;
	while (pThis->m_bWorking)
	{
		pThis->m_lckForSendMap.Lock();

		for (auto& pi:pThis->m_mapSend)
		{
			//û�з����İ�,����
			//��ʱ�İ�������
			ULONGLONG tmCurrent = GetTickCount64();
			if (tmCurrent - pi.second.m_tmLastTime > pThis->m_tmElapse)
			{
				if (pi.second.m_tmLastTime == NULL)
				{
					pThis->Log("[umt]: package ==> ��һ�η������ݰ�: %d",pi.second.m_pkg.m_nSeq);
				}
				else
				{
					pThis->Log("[umt]: package ==> �ط����ݰ�: %d", pi.second.m_pkg.m_nSeq);
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

		//�г��߳�
		Sleep(1);
	}
	return NULL;
}

DWORD CALLBACK CUMT::RecvThread(LPVOID lpParam)
{
	SetThreadPriority(GetCurrentThread(), REALTIME_PRIORITY_CLASS);		//ʵʱ
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


		//�жϰ�������
		switch (pkg.m_nPt)
		{
		case PT_ACK:
		{
			pThis->m_lckForSendMap.Lock();
			pThis->Log("[umt]: ����[ack]��%d", pkg.m_nSeq);
			pThis->m_mapSend.erase(pkg.m_nSeq);			//�����ݰ��ӷ����������߳�
			pThis->m_lckForSendMap.UnLock();
			break;
		}
		case PT_DATA:
		{
			//У��
			DWORD dwCheck = CCrc32::crc32(pkg.m_aryData, pkg.m_nLen);

			//У��ʧ��
			if (dwCheck != pkg.m_nCheck)
			{
				//������
				break;
			}

			//У��ɹ����ظ�ACK
			CPackage pkgAck(PT_ACK, pkg.m_nSeq);
			sendto(pThis->m_sock,
				(char*)&pkgAck, sizeof(pkgAck),
				0,
				(sockaddr*)&pThis->m_siDst, sizeof(pThis->m_siDst)
			);

			//��������
			pThis->m_lckForRecvMap.Lock();

			if (pThis->m_mapRecv.find(pkg.m_nSeq) != pThis->m_mapRecv.end()	//�����д���Ŵ���(���������)
				|| pkg.m_nSeq < pThis->m_nNextRecvSeq		//����ŵİ��е������Ѿ����뻺����
				)
			{
				pThis->m_lckForRecvMap.UnLock();
				break;
			}
			pThis->m_mapRecv[pkg.m_nSeq] = pkg;
			pThis->Log("[umt]:�����ݰ���������");
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
				//����������
				auto& pkg = pThis->m_mapRecv[pThis->m_nNextRecvSeq];
				pThis->m_bufRecv.Write(pkg.m_aryData, pkg.m_nLen);

				pThis->Log("[umt]: ���������뻺����:%d", pThis->m_nNextRecvSeq);
				
				//���������Ƴ���
				pThis->m_mapRecv.erase(pThis->m_nNextRecvSeq);

				//��Ÿ���
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

		//�г��߳�
		Sleep(1);
	}
	return NULL;
}
