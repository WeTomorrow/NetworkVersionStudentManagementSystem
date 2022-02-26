#pragma once
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")

#include <map>
using namespace std;
#include "ByteStreamBuff.h"
#include "CLock.h"
#include "CCrc32.h"

#define DATALEN 1460		//���ݳ���

//UDPģ��TCP
class CUMT
{
public:
	~CUMT();

	//ʵ�ֽӿ�,�������Ϳͻ���ͨ��
	
	//�������������������ݵĽӿ�
	BOOL Accept(LPCTSTR szIp, USHORT nPort);

	//���ӷ������Ľӿ�
	BOOL Connect(LPCTSTR szIp,USHORT nPort);

	void Log(const char* szFmt, ...);

	//�������ݵĽӿ�
	DWORD Send(LPBYTE pBuff, DWORD dwBufLen);		

	//�������ݵĽӿ�
	DWORD Recv(LPBYTE pBuff, DWORD dwBufLen);

	//�ر����ӵĽӿ�
	VOID Close();
private:
	//˽�г�Ա����

	//�������ӳ�ʼ�������߳�
	BOOL Init();
	
	//���������߳�
	VOID Clear();	

private:
	//����ʽ:
#pragma region ����ʽ�ṹ��

	enum PackageType { 
		PT_DATA = 1, 
		PT_ACK = 2, 
		PT_FIN = 3,
		PT_SYN = 4 };	//������

#pragma pack(push)
#pragma pack(1)
	struct CPackage
	{
		CPackage() {};
		CPackage(WORD pt,DWORD dwSeq,LPBYTE pData = NULL,WORD nDataLen = NULL) 
		{
			m_nPt = pt;
			m_nSeq = dwSeq;
			m_nLen = NULL;
			m_nCheck = NULL;
			if (pData != NULL)
			{
				memcpy(m_aryData, pData, nDataLen);
				m_nLen = nDataLen;
				m_nCheck = CCrc32::crc32(pData, nDataLen);
			}
		};

		WORD m_nPt; //��������
		WORD m_nLen;//���ݵĳ���
		DWORD m_nSeq; //�������
		DWORD m_nCheck;//�����ݵ�Crc32У��ֵ
		BYTE m_aryData[DATALEN];		//����
	};
#pragma pack(pop)

	struct CPackageInfo				//���պ�����ݰ��ṹ�壬������ʱ��
	{
		CPackageInfo() {};
		CPackageInfo(time_t tm,CPackage pkg) :
			m_tmLastTime(tm),
			m_pkg(pkg)
		{};
		ULONGLONG m_tmLastTime;		//����ʱ��,��Ҫ�ڽ���ʱ��ȡ
		CPackage m_pkg;				//���ݰ�����Ϣ
	};

#pragma endregion
private:
	//��ͨ���ͳ�Ա
	DWORD m_nNextSendSeq = NULL;		//��һ�β���Ŀ�ʼ���
	DWORD m_nNextRecvSeq = NULL;		//��һ�δ��뻺�����İ������
private:
	//MFC��socket��Ա
	SOCKET m_sock = NULL;		

	SOCKADDR_IN m_siDst = {};		//�Է��ĵ�ַ
	SOCKADDR_IN m_siSrc = {};		//�Լ��ĵ�ַ

	const ULONGLONG m_tmElapse = 500;	//��ʱʱ��

private:
	//�Զ������ͳ�Ա
	map<DWORD, CPackageInfo> m_mapSend;		//�洢���Ͱ�������
	map<DWORD, CPackage> m_mapRecv;		//�洢�յ��İ�

	CByteStreamBuff m_bufRecv;	//�������ݵĻ�����

	CLock m_lckForSendMap;	//ͬ������(�ٽ���),����m_mapSend�Ķ��߳�
	CLock m_lckForRecvMap;	//ͬ������(�ٽ���),����m_mapRecv�Ķ��߳�
	CLock m_lckForBufRecv;	//ͬ������(�ٽ���),����m_bufRecv�Ķ��߳�
private:
	//�߳̾��
	HANDLE m_hSendThread = NULL;
	HANDLE m_hRecvThreadA = NULL;
	HANDLE m_hRecvThreadB = NULL;
	HANDLE m_hRecvThreadC = NULL;
	HANDLE m_hHandleThread = NULL;

	BOOL m_bWorking = FALSE;	//���߳���

	//�߳�
	static DWORD CALLBACK SendThread(LPVOID lpParam);	//�������߳�

	static DWORD CALLBACK RecvThread(LPVOID lpParam);	//�հ����߳�
	static DWORD CALLBACK HandleRecvPkgsThread(LPVOID lpParam);		//�������հ��������뻺�������߳�

};

