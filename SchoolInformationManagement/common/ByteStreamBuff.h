#pragma once
#include <vector>
#include <winscard.h>
using namespace std;

//�򵥵��ֽ���������
class CByteStreamBuff
{
public:
	CByteStreamBuff();
	~CByteStreamBuff();

	//д�뻺����, �����ڻ�����ĩβ
	void Write(LPCBYTE pBuff, int nSize);
	//�ӻ�������ȡָ���ֽ���, ��ȡ�������Զ��ӻ�����ɾ��
	void Read(LPBYTE pBuff, int nSize);
	//�ӻ�������ȡָ���ֽ���, ��ȡ�����ݲ���ӻ�����ɾ��
	void Peek(LPBYTE pBuff, int nSize);
	//��ȡ�����������ݵĴ�С
	int GetSize()const;
	
private:
	vector<char> m_vctBuff;
};
