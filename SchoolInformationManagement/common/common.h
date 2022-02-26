#pragma once
#include <atltypes.h>
#include <windows.h>

enum PkgServer//选课记录
{

    //服务端返回给客户端，操作后的表数据信息展示
    S2C_CLASS,     //接受服务端发送到客户端的班级的包
    S2C_STUDENT,   //接受服务端发送到客户端的学生的包
    S2C_COURSE,    //接受服务端发送到客户端的课程的包
    S2C_CS,        //接受服务端发送到客户端的选课记录的包
    S2C_QUERYCLS,    //查询模式班级号
    S2C_QUERY2,    //查询模式2
    S2C_QUERY3,    //查询模式3
    S2C_QUERY4     //查询模式4
};

enum STUDENT1
{
    //客户端传给服务端
    STU_ADD,    //添加
    STU_DELETE, //删除
    STU_LATER,  //修改
    STU_QUERY1, //查询学生id
    STU_QUERY2, //查询学生姓名
    STU_QUERY3, //查询学生id和学生姓名

    //服务端返回给客户端，操作后的表数据信息展示
    STU_OTHER,    //其他操作返回操作后的表数据
    STU_QUERY4, //查询学生id
    STU_QUERY5, //查询学生姓名
    STU_QUERY6, //查询学生id和学生姓名
};

enum COURSE1
{
    //客户端传给服务端
    COU_ADD,    //添加
    COU_DELETE, //删除
    COU_LATER,  //修改
    COU_QUERY1, //查询课程id
    COU_QUERY2, //查询课程名
    COU_QUERY3, //查询课程id和课程名
    
    //服务端返回给客户端，操作后的表数据信息展示
    COU_OTHER,    //其他操作返回操作后的表数据
    COU_QUERY4, //查询课程id
    COU_QUERY5, //查询课程名
    COU_QUERY6, //查询课程id和课程名
};
enum CLASS1
{
    //客户端传给服务端
    CLS_ADD,    //添加
    CLS_DELETE, //删除
    CLS_LATER,  //修改
    CLS_QUERY1, //查询班级id

    //服务端返回给客户端，操作后的表数据信息展示
    CLS_OTHER,    //其他操作返回操作后的表数据
    CLS_QUERY2, //查询班级id
};

#pragma pack(push)
//#pragma pack(1)
//typedef struct tagPkgCS
//{
//    CS1 m_cs;          //数据类型
//    DWORD m_nLen;         //数据长度
//    BYTE m_btData[1];    //柔性数组
//}PKGCS, *PPKGCS;

typedef struct tagStudent
{
    STUDENT1 m_stu;          //数据类型
    DWORD m_nLen;         //数据长度
    BYTE m_btData[1];    //柔性数组
}STUDENT, * PSTUDENT;

typedef struct tagClass
{
    DWORD m_clsid;         //班级
}CLASS, * PCLASS;
 //PCLASS pClass = new tagClass[255];
 typedef struct tagPkgSrever
 {
     PkgServer m_type;  //数据类型
     DWORD m_nLen;      //数据长度
     BYTE m_btData[255];    //柔性数组
 }PKGSERVER, * PPKGSERVER;



typedef struct tagCourse
{
    COURSE1 m_cou;          //数据类型
    DWORD m_nLen;         //数据长度
    BYTE m_btData[1];    //柔性数组
}COURSE, * PCOURSE;


#pragma pack(pop)