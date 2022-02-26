#pragma once
#include <atltypes.h>
#include <windows.h>

enum PkgServer//ѡ�μ�¼
{

    //����˷��ظ��ͻ��ˣ�������ı�������Ϣչʾ
    S2C_CLASS,     //���ܷ���˷��͵��ͻ��˵İ༶�İ�
    S2C_STUDENT,   //���ܷ���˷��͵��ͻ��˵�ѧ���İ�
    S2C_COURSE,    //���ܷ���˷��͵��ͻ��˵Ŀγ̵İ�
    S2C_CS,        //���ܷ���˷��͵��ͻ��˵�ѡ�μ�¼�İ�
    S2C_QUERYCLS,    //��ѯģʽ�༶��
    S2C_QUERY2,    //��ѯģʽ2
    S2C_QUERY3,    //��ѯģʽ3
    S2C_QUERY4     //��ѯģʽ4
};

enum STUDENT1
{
    //�ͻ��˴��������
    STU_ADD,    //���
    STU_DELETE, //ɾ��
    STU_LATER,  //�޸�
    STU_QUERY1, //��ѯѧ��id
    STU_QUERY2, //��ѯѧ������
    STU_QUERY3, //��ѯѧ��id��ѧ������

    //����˷��ظ��ͻ��ˣ�������ı�������Ϣչʾ
    STU_OTHER,    //�����������ز�����ı�����
    STU_QUERY4, //��ѯѧ��id
    STU_QUERY5, //��ѯѧ������
    STU_QUERY6, //��ѯѧ��id��ѧ������
};

enum COURSE1
{
    //�ͻ��˴��������
    COU_ADD,    //���
    COU_DELETE, //ɾ��
    COU_LATER,  //�޸�
    COU_QUERY1, //��ѯ�γ�id
    COU_QUERY2, //��ѯ�γ���
    COU_QUERY3, //��ѯ�γ�id�Ϳγ���
    
    //����˷��ظ��ͻ��ˣ�������ı�������Ϣչʾ
    COU_OTHER,    //�����������ز�����ı�����
    COU_QUERY4, //��ѯ�γ�id
    COU_QUERY5, //��ѯ�γ���
    COU_QUERY6, //��ѯ�γ�id�Ϳγ���
};
enum CLASS1
{
    //�ͻ��˴��������
    CLS_ADD,    //���
    CLS_DELETE, //ɾ��
    CLS_LATER,  //�޸�
    CLS_QUERY1, //��ѯ�༶id

    //����˷��ظ��ͻ��ˣ�������ı�������Ϣչʾ
    CLS_OTHER,    //�����������ز�����ı�����
    CLS_QUERY2, //��ѯ�༶id
};

#pragma pack(push)
//#pragma pack(1)
//typedef struct tagPkgCS
//{
//    CS1 m_cs;          //��������
//    DWORD m_nLen;         //���ݳ���
//    BYTE m_btData[1];    //��������
//}PKGCS, *PPKGCS;

typedef struct tagStudent
{
    STUDENT1 m_stu;          //��������
    DWORD m_nLen;         //���ݳ���
    BYTE m_btData[1];    //��������
}STUDENT, * PSTUDENT;

typedef struct tagClass
{
    DWORD m_clsid;         //�༶
}CLASS, * PCLASS;
 //PCLASS pClass = new tagClass[255];
 typedef struct tagPkgSrever
 {
     PkgServer m_type;  //��������
     DWORD m_nLen;      //���ݳ���
     BYTE m_btData[255];    //��������
 }PKGSERVER, * PPKGSERVER;



typedef struct tagCourse
{
    COURSE1 m_cou;          //��������
    DWORD m_nLen;         //���ݳ���
    BYTE m_btData[1];    //��������
}COURSE, * PCOURSE;


#pragma pack(pop)