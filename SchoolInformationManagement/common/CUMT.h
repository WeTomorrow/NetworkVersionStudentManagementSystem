#pragma once
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")

#include <map>
using namespace std;
#include "ByteStreamBuff.h"
#include "CLock.h"
#include "CCrc32.h"

#define DATALEN 1460		//数据长度

//UDP模拟TCP
class CUMT
{
public:
	~CUMT();

	//实现接口,服务器和客户端通用
	
	//服务器监听并接受数据的接口
	BOOL Accept(LPCTSTR szIp, USHORT nPort);

	//连接服务器的接口
	BOOL Connect(LPCTSTR szIp,USHORT nPort);

	void Log(const char* szFmt, ...);

	//发送数据的接口
	DWORD Send(LPBYTE pBuff, DWORD dwBufLen);		

	//接受数据的接口
	DWORD Recv(LPBYTE pBuff, DWORD dwBufLen);

	//关闭连接的接口
	VOID Close();
private:
	//私有成员函数

	//建立连接初始化所有线程
	BOOL Init();
	
	//清理所有线程
	VOID Clear();	

private:
	//包格式:
#pragma region 包格式结构体

	enum PackageType { 
		PT_DATA = 1, 
		PT_ACK = 2, 
		PT_FIN = 3,
		PT_SYN = 4 };	//包类型

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

		WORD m_nPt; //包的类型
		WORD m_nLen;//数据的长度
		DWORD m_nSeq; //包的序号
		DWORD m_nCheck;//包数据的Crc32校验值
		BYTE m_aryData[DATALEN];		//数据
	};
#pragma pack(pop)

	struct CPackageInfo				//接收后的数据包结构体，包括了时间
	{
		CPackageInfo() {};
		CPackageInfo(time_t tm,CPackage pkg) :
			m_tmLastTime(tm),
			m_pkg(pkg)
		{};
		ULONGLONG m_tmLastTime;		//包的时间,需要在接受时获取
		CPackage m_pkg;				//数据包的信息
	};

#pragma endregion
private:
	//普通类型成员
	DWORD m_nNextSendSeq = NULL;		//下一次拆包的开始序号
	DWORD m_nNextRecvSeq = NULL;		//下一次存入缓冲区的包的序号
private:
	//MFC与socket成员
	SOCKET m_sock = NULL;		

	SOCKADDR_IN m_siDst = {};		//对方的地址
	SOCKADDR_IN m_siSrc = {};		//自己的地址

	const ULONGLONG m_tmElapse = 500;	//超时时间

private:
	//自定义类型成员
	map<DWORD, CPackageInfo> m_mapSend;		//存储发送包的容器
	map<DWORD, CPackage> m_mapRecv;		//存储收到的包

	CByteStreamBuff m_bufRecv;	//接收数据的缓冲区

	CLock m_lckForSendMap;	//同步对象(临界区),用于m_mapSend的多线程
	CLock m_lckForRecvMap;	//同步对象(临界区),用于m_mapRecv的多线程
	CLock m_lckForBufRecv;	//同步对象(临界区),用于m_bufRecv的多线程
private:
	//线程句柄
	HANDLE m_hSendThread = NULL;
	HANDLE m_hRecvThreadA = NULL;
	HANDLE m_hRecvThreadB = NULL;
	HANDLE m_hRecvThreadC = NULL;
	HANDLE m_hHandleThread = NULL;

	BOOL m_bWorking = FALSE;	//退线程用

	//线程
	static DWORD CALLBACK SendThread(LPVOID lpParam);	//发包的线程

	static DWORD CALLBACK RecvThread(LPVOID lpParam);	//收包的线程
	static DWORD CALLBACK HandleRecvPkgsThread(LPVOID lpParam);		//将包从收包容器放入缓冲区的线程

};

