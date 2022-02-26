
// SchoolInformationManagementClientDlg.h: 头文件
//

#pragma once

#include "../common/common.h"
#include "../common/CLock.h"
#include "../common/CUMT.h"
#include <string.h>

// CSchoolInformationManagementClientDlg 对话框
class CSchoolInformationManagementClientDlg : public CDialogEx
{
public:
    CUMT m_cumt;
    SOCKET m_socketControl;
    //获取窗口句柄
    HWND m_hwndDlg;
// 构造
public:
	CSchoolInformationManagementClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCHOOLINFORMATIONMANAGEMENTCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedControlconnect();
	afx_msg void OnBnClickedCsadd();
	afx_msg void OnBnClickedClsadd();
	afx_msg void OnBnClickedClsalter();
	afx_msg void OnBnClickedClsquery();
};
