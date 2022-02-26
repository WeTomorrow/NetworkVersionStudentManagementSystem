// SchoolInformationManagementServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../common/CUMT.h"
#include "include/mysql.h"
#pragma comment(lib, "lib/libmysql.lib")
#include <string.h>
#include "../common/common.h"
//  using namespace std;

int g_nrowLen = 0;//计算行的个数
int main()
{
    MYSQL* mysql = mysql_init(NULL);

    //连接mysql服务器
    MYSQL* conn = mysql_real_connect(mysql,
        NULL, //主机ip， NULL为本地
        "root",
        "toor",
        "test6",//schoolinformationmanagement
        3306,
        NULL,
        0);
    if (conn == NULL)
    {
        cout << "connect failed:" << mysql_error(mysql) << endl;
        return 0;
    }
    mysql_set_character_set(conn, "gbk"); //通知mysql，返回的结果转换成gbk编码



     char szClassTable[] = { "create table class(clsid INT UNSIGNED PRIMARY KEY auto_increment);" };
     DWORD dwClassSqlLen = sizeof(szClassTable);

     char szCourseTable[] = { "create table course(couid INT UNSIGNED PRIMARY KEY auto_increment, couname varchar(255));" };
     DWORD dwCourseSqlLen = sizeof(szCourseTable);

     char szStudentTable[] = { "create table student(stuid INT UNSIGNED PRIMARY KEY auto_increment, stuname varchar(255), clsid INT UNSIGNED, foreign key(clsid) REFERENCES class(clsid) on delete cascade on update  cascade);" }; 
     DWORD dwStudentSqlLen = sizeof(szStudentTable);

     char szCSTable[] = { "create table courseselect(stuid INT UNSIGNED not null ,couid INT UNSIGNED not null,primary key(stuid, couid),foreign key(stuid) REFERENCES student(stuid) on delete cascade on update  cascade,foreign key(couid) REFERENCES course(couid) on delete cascade on update  cascade);" };
     DWORD dwCSSqlLen = sizeof(szCSTable);

     //先建表
     int nRet = mysql_real_query(conn, szClassTable, dwClassSqlLen);
     if (nRet != 0)
     {
         cout << "query failed" << mysql_error(mysql) << endl;
     }

     nRet = mysql_real_query(conn, szCourseTable, dwCourseSqlLen);
     if (nRet != 0)
     {
         cout << "query failed" << mysql_error(mysql) << endl;
     }
     nRet = mysql_real_query(conn, szStudentTable, dwStudentSqlLen);
     if (nRet != 0)
     {
         cout << "query failed" << mysql_error(mysql) << endl;
     }
     nRet = mysql_real_query(conn, szCSTable, dwCSSqlLen);
     if (nRet != 0)
     {
         cout << "query failed" << mysql_error(mysql) << endl;
     }



     //与客户端建立连接
     CUMT cumt;
     cumt.Accept((LPCTSTR)"0.0.0.0", 7788);//虚拟机192.168.50.128 本机192.168.16.163

     while (true)
     {
         PKGSERVER pkgData;
         pkgData.m_type = S2C_QUERYCLS;

         DWORD dwSqlDataSize = 0;//接收客户端发送的Sql语句的大小
         cumt.Recv((LPBYTE)&dwSqlDataSize, sizeof(dwSqlDataSize));

         //接收客户端发送的Sql语句

         DWORD dwRet = cumt.Recv((LPBYTE)&pkgData, dwSqlDataSize);

         LPBYTE pBuff = new BYTE[pkgData.m_nLen];
         memcpy(pBuff, pkgData.m_btData, pkgData.m_nLen);
         //执行sql语句
         //mysql_set_character_set(conn, "gbk");

         nRet = mysql_real_query(conn, (const char*)pBuff, pkgData.m_nLen);
         if (nRet != 0)
         {
             cout << "query failed" << mysql_error(mysql) << endl;
             continue;
         }

         switch (pkgData.m_type)
         {
         case S2C_CLASS:
             break;
         case S2C_STUDENT:
             break;
         case S2C_COURSE:
             break;
         case S2C_CS:
             break;
         case S2C_QUERYCLS:
         {


             PCLASS pClass = new tagClass[255];
             //表头信息
             MYSQL_RES* result = mysql_store_result(conn);
             int nNumOfFields = mysql_num_fields(result);
             MYSQL_FIELD* fields = mysql_fetch_fields(result); //获取表头信息
             /*for (int i = 0; i < nNumOfFields; ++i)
             {
                 cout << i << ":" << fields[i].name << endl;
             }*/

             //获取行
             MYSQL_ROW row = NULL;
             while ((row = mysql_fetch_row(result)) != NULL)
             {
                 for (int i = 0; i < nNumOfFields; ++i)
                 {
                     pClass->m_clsid = atoi(row[i]);
                     cout << (row[i] == NULL ? "" : row[i]) << " \t";
                 }

                 cout << endl;
                 pClass[g_nrowLen++];
                 //nrowLen++;
             }

             //将查询的的数据发回给客户端
             DWORD pkgLen = sizeof(tagClass) * g_nrowLen;
             cumt.Send((LPBYTE)&pkgLen, sizeof(pkgLen));
             //Sleep(1);
             cumt.Send((LPBYTE)&pClass, sizeof(pClass));
             delete[] pClass;
             break;
         }
         case S2C_QUERY2:
             break;
         case S2C_QUERY3:
             break;
         case S2C_QUERY4:
             break;
         default:
             break;
         }



         delete[] pBuff;
     }
 
 


    //执行sql语句
    //char szSqlText[] = { "select * from t_stu;" };
 /*   char szSqlText[] = { "select * from t_stu where name = '张三';" };
    int nRet = mysql_real_query(conn, szSqlText, sizeof(szSqlText));
    if (nRet != 0)
    {
        cout << "query failed" << mysql_error(mysql) << endl;
    }*/

    //表头信息
    MYSQL_RES* result = mysql_store_result(conn);
    int nNumOfFields = mysql_num_fields(result);
    MYSQL_FIELD* fields = mysql_fetch_fields(result); //获取表头信息
    for (int i = 0; i < nNumOfFields; ++i)
    {
        cout << i << ":" << fields[i].name << endl;
    }

    //获取行
    MYSQL_ROW row = NULL;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        for (int i = 0; i < nNumOfFields; ++i)
        {
            cout << (row[i] == NULL ? "" : row[i]) << " \t";
        }

        cout << endl;
    }

    cumt.Close();
    mysql_close(mysql);
    return 0;
}
