
// SchoolInformationManagementClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SchoolInformationManagementClient.h"
#include "SchoolInformationManagementClientDlg.h"
#include "afxdialogex.h"

#include "../common/common.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void InsertColumn(HWND hDlg, DWORD dwCtrlId, LPTSTR szText, int nLen, DWORD dwColIdx)
{
    LVCOLUMN lvc = {};
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 50;
    lvc.pszText = szText;
    lvc.cchTextMax = nLen;
    SendDlgItemMessage(
        hDlg,
        dwCtrlId,
        LVM_INSERTCOLUMN,
        dwColIdx,
        (LPARAM)&lvc);

}

//获取一行数据
void GetItemText(HWND hDlg, DWORD dwCtrlId, LPTSTR szText, int nLen, DWORD dwRowIdx, DWORD dwColIdx)
{
    LVITEM lviText = {};
    lviText.mask = LVIF_TEXT;
    lviText.iItem = dwRowIdx;
    lviText.iSubItem = dwColIdx;
    lviText.pszText = szText;
    lviText.cchTextMax = nLen;
    SendDlgItemMessage(
        hDlg,
        dwCtrlId,
        LVM_GETITEMTEXT,
        dwRowIdx,
        (LPARAM)&lviText);
}

//设置一行数据
void SetItemText(HWND hDlg, DWORD dwCtrlId, LPTSTR szText, int nLen, DWORD dwRowIdx, DWORD dwColIdx)
{
    LVITEM lviText = {};
    lviText.mask = LVIF_TEXT;
    lviText.iItem = dwRowIdx;
    lviText.iSubItem = dwColIdx;
    lviText.pszText = szText;
    lviText.cchTextMax = nLen;
    SendDlgItemMessage(
        hDlg,
        dwCtrlId,
        LVM_SETITEMTEXT,
        dwRowIdx,
        (LPARAM)&lviText);
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSchoolInformationManagementClientDlg 对话框



CSchoolInformationManagementClientDlg::CSchoolInformationManagementClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SCHOOLINFORMATIONMANAGEMENTCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSchoolInformationManagementClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSchoolInformationManagementClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BTN_CONTROLCONNECT, &CSchoolInformationManagementClientDlg::OnBnClickedControlconnect)
	ON_BN_CLICKED(BTN_CSADD, &CSchoolInformationManagementClientDlg::OnBnClickedCsadd)
	ON_BN_CLICKED(BTN_CLSADD, &CSchoolInformationManagementClientDlg::OnBnClickedClsadd)
	ON_BN_CLICKED(BTN_CLSALTER, &CSchoolInformationManagementClientDlg::OnBnClickedClsalter)
	ON_BN_CLICKED(BTN_CLSQUERY, &CSchoolInformationManagementClientDlg::OnBnClickedClsquery)
END_MESSAGE_MAP()


// CSchoolInformationManagementClientDlg 消息处理程序

BOOL CSchoolInformationManagementClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    //插入列，选课记录
	m_hwndDlg = this->GetSafeHwnd();
    TCHAR szCSID[] = TEXT("选课ID");
    InsertColumn(m_hwndDlg, IDC_LISTCS, szCSID, sizeof(szCSID), 0);
    TCHAR szStuID[] = TEXT("学生ID");
    InsertColumn(m_hwndDlg, IDC_LISTCS, szStuID, sizeof(szStuID), 1);
    TCHAR szCSName[] = TEXT("学生名");
    InsertColumn(m_hwndDlg, IDC_LISTCS, szCSName, sizeof(szCSName), 2);
    TCHAR szClsID[] = TEXT("班级");
    InsertColumn(m_hwndDlg, IDC_LISTCS, szClsID, sizeof(szClsID), 3);
    TCHAR szCouID[] = TEXT("课程ID");
    InsertColumn(m_hwndDlg, IDC_LISTCS, szCouID, sizeof(szCouID), 4);
    TCHAR szCouName[] = TEXT("课程名");
    InsertColumn(m_hwndDlg, IDC_LISTCS, szCouName, sizeof(szCouName), 5);

    //插入列，班级
    InsertColumn(m_hwndDlg, IDC_LISTCLS, szClsID, sizeof(szClsID), 0);

    //插入列，课程
    InsertColumn(m_hwndDlg, IDC_LISTCOU, szCouID, sizeof(szCouID), 0);
    InsertColumn(m_hwndDlg, IDC_LISTCOU, szCouName, sizeof(szCouName), 1);
	
    //插入列，学生
    InsertColumn(m_hwndDlg, IDC_LISTSTU, szStuID, sizeof(szStuID), 0);
    InsertColumn(m_hwndDlg, IDC_LISTSTU, szCSName, sizeof(szCSName), 1);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSchoolInformationManagementClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSchoolInformationManagementClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSchoolInformationManagementClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSchoolInformationManagementClientDlg::OnBnClickedControlconnect()//建立连接
{
	BOOL nbool = m_cumt.Connect((LPCTSTR)"127.0.0.1", 7788);//虚拟机192.168.50.128 本机192.168.16.163
	if (nbool == FALSE)
	{
		AfxMessageBox((LPCTSTR)"建立连接失败！");
	}
	else
	{

        AfxMessageBox((LPCTSTR)"建立连接成功！");
	}
	// TODO: 在此添加控件通知处理程序代码
}

//添加选课记录
void CSchoolInformationManagementClientDlg::OnBnClickedCsadd()
{
	// TODO: 在此添加控件通知处理程序代码

}


void CSchoolInformationManagementClientDlg::OnBnClickedClsadd()
{
    //获取名字
    CString strName;
	//获取文本框数据
	GetDlgItemText(TXT_CLSID, strName);
	if (strName.GetLength() <= 0)
	{
		AfxMessageBox("请输入数据");
		return;
	}
    CString strsql1("INSERT INTO class VALUES('");
    CString strsql2("');");
	CString strSql;
    strSql = strsql1 + strName + strsql2+'\0';

	DWORD nLen = strSql.GetLength();
	char szsql[256] = {};
	memcpy(szsql, strSql.GetBuffer(), strSql.GetLength());

    //   CLASS cls;
 //   cls.m_cls = CLS_ADD;
 //   cls.m_nLen = nLen;
	//memcpy(cls.m_btData, strSql, nLen);

    m_cumt.Send((LPBYTE)&nLen, sizeof(nLen));
	//Sleep(1);
	m_cumt.Send((LPBYTE)szsql, nLen);
	// TODO: 在此添加控件通知处理程序代码
}


void CSchoolInformationManagementClientDlg::OnBnClickedClsalter()
{
	//获取列表的一行
    TCHAR szName[MAXBYTE] = TEXT("\0");
    DWORD dwSel = SendDlgItemMessage(IDC_LISTCS, LVM_GETSELECTIONMARK,  0,0);//LVM_GETSELECTIONMARK,
    SendDlgItemMessageA(IDC_LISTCS, LVM_GETSELECTIONMARK, dwSel, 0);//LVM_DELETEITEM
    GetItemText(m_hwndDlg, IDC_LISTCS, szName, sizeof(szName), dwSel,0);


	//获取文本框数据
    CString strName;
    //获取文本框数据
    GetDlgItemText(TXT_CLSID2, strName);
    if (strName.GetLength() <= 0)
    {
        AfxMessageBox("请输入数据");
        return;
    }
    CString strsql1("INSERT INTO class VALUES('");
    CString strsql2("');");
    CString strSql;
    strSql = strsql1 + strName + strsql2 + '\0';

    DWORD nLen = strSql.GetLength();
    char szsql[256] = {};
    memcpy(szsql, strSql.GetBuffer(), strSql.GetLength());

    //   CLASS cls;
 //   cls.m_cls = CLS_ADD;
 //   cls.m_nLen = nLen;
    //memcpy(cls.m_btData, strSql, nLen);

    m_cumt.Send((LPBYTE)&nLen, sizeof(nLen));
    //Sleep(1);
    m_cumt.Send((LPBYTE)szsql, nLen);
	// TODO: 在此添加控件通知处理程序代码
}


void CSchoolInformationManagementClientDlg::OnBnClickedClsquery()
{
    //获取文本框数据
    CString strName;
    GetDlgItemText(TXT_CLSID2, strName);

    CString strsql3("SELECT *FROM class;" + '\0');
    CString strsql1("SELECT *FROM class where clsid = ");
    CString strsql2("';");
    CString strSql;
    //strSql = strsql1 + strName + strsql2 + '\0';

    if (strName.GetLength() <= 0)
    {
        strSql = strsql3;//如果文本框没有数据查询整张表
    }
    else
    {

        strSql = strsql1 + strName + strsql2 + '\0';//如果文本框有数据查询该数据
    }

    DWORD nLen = strSql.GetLength();
    char szsql[256] = {};
    memcpy(szsql, strSql.GetBuffer(), strSql.GetLength());


    PKGSERVER pkgData;
    pkgData.m_type = S2C_QUERYCLS;
    pkgData.m_nLen = nLen;
    memcpy(pkgData.m_btData, szsql, pkgData.m_nLen);
    DWORD npkgDataSize = sizeof(pkgData);
    m_cumt.Send((LPBYTE)&npkgDataSize, sizeof(npkgDataSize));
    //Sleep(1);
    m_cumt.Send((LPBYTE)&pkgData, sizeof(pkgData));


    //接收执行后的信息反馈
    PCLASS pClass = new tagClass[255];
    DWORD dwSqlDataSize = 0;//接收客户端发送的Sql语句的大小
    m_cumt.Recv((LPBYTE)&dwSqlDataSize, sizeof(dwSqlDataSize));

    //接收客户端发送的Sql语句
    int* nrow = new int[dwSqlDataSize];

    

    /*for (int i = 0;pClass[i].m_clsid != NULL;i++)
    {
        char* szBuff = {};
        ltoa(pClass->m_clsid, szBuff, 10);
        SetItemText(m_hwndDlg, TXT_CLSID2, szBuff, sizeof(szBuff), i, 0);
    }*/
	// TODO: 在此添加控件通知处理程序代码
}
