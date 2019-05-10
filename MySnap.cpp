// MySnap.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MySnap.h"
#include "MainFrm.h"

#include "MySnapDoc.h"
#include "MySnapView.h"
#include ".\mysnap.h"
#include "MyDataDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CMySnapApp

BEGIN_MESSAGE_MAP(CMySnapApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMySnapApp 构造

CMySnapApp::CMySnapApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	//---------- get application's path -----------------
	TCHAR exeFullPath[MAX_PATH];
    GetModuleFileName(NULL,exeFullPath,MAX_PATH);
	CString strTmp	= _T(exeFullPath);
	int index		= strTmp.ReverseFind('\\');
	m_strAppPath	= strTmp.Left(index);
	//---------- end of get -----------------------------

	//to use drawDIB
	m_hDrawDib= DrawDibOpen();

}

CMySnapApp::~CMySnapApp()
{
	if( m_hDrawDib != NULL)
	{
		DrawDibClose( m_hDrawDib);
		m_hDrawDib = NULL;
	}
}
// 唯一的一个 CMySnapApp 对象

CMySnapApp theApp;

// CMySnapApp 初始化

BOOL CMySnapApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
//	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
//	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMySnapDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CMySnapView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	//create snapinfo dialog
	m_dlgSnapInfo.Create(IDD_SNAPINFO);
//	m_dlgSnapInfo.ShowWindow(SW_HIDE);

	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当存在后缀时才调用 DragAcceptFiles，
	// 在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生



	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedHomepage();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_STN_CLICKED(IDC_HOMEPAGE, OnStnClickedHomepage)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CMySnapApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMySnapApp 消息处理程序


int CMySnapApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::ExitInstance();
}

//show snap infomation dialog.
void CMySnapApp::ShowSnapInfoDlg(CPoint mp, COLORREF clr, int currentStatus)
{
	int oldX = m_dlgSnapInfo.m_rect.left;
	int oldY = m_dlgSnapInfo.m_rect.top;
	int newX = m_dlgSnapInfo.m_rect.left;
	int newY = m_dlgSnapInfo.m_rect.top;

	//is capturint now, so we only need to consider the selecting region
	if (m_pView->m_isCapturing)
	{
		if ( newX == 0 
			&& newY == 0 
			&& mp.x <= m_dlgSnapInfo.m_rect.Width() + MOUSE_SENSITIVITY
			&& mp.y <= m_dlgSnapInfo.m_rect.Height() + MOUSE_SENSITIVITY
			)
		{
			newX = GetSystemMetrics(SM_CXSCREEN) - m_dlgSnapInfo.m_rect.Width();
			newY = GetSystemMetrics(SM_CYSCREEN) - m_dlgSnapInfo.m_rect.Height();
		}
		else if ( newX != 0//== GetSystemMetrics(SM_CXSCREEN) - m_dlgSnapInfo.m_rect.Width()
				&& newY == 0
				&& mp.x >= GetSystemMetrics(SM_CXSCREEN) - m_dlgSnapInfo.m_rect.Width() - MOUSE_SENSITIVITY
				&& mp.y <= m_dlgSnapInfo.m_rect.Height() + MOUSE_SENSITIVITY
				)
		{
			newX = 0;
			newY = GetSystemMetrics(SM_CYSCREEN) - m_dlgSnapInfo.m_rect.Height();
		}
		else if ( newX == 0
				&& newY != 0//== GetSystemMetrics(SM_CYSCREEN) - m_dlgSnapInfo.m_rect.Height()
				&& mp.x <= m_dlgSnapInfo.m_rect.Width() + MOUSE_SENSITIVITY
				&& mp.y >= GetSystemMetrics(SM_CYSCREEN) - m_dlgSnapInfo.m_rect.Height() - MOUSE_SENSITIVITY
				)
		{
			newX = GetSystemMetrics(SM_CXSCREEN) - m_dlgSnapInfo.m_rect.Width();
			newY = 0;
		}
		else if ( newX != 0//== GetSystemMetrics(SM_CXSCREEN) - m_dlgSnapInfo.m_rect.Width()
				&& newY != 0//== GetSystemMetrics(SM_CYSCREEN) - m_dlgSnapInfo.m_rect.Height()
				&& mp.x >= GetSystemMetrics(SM_CXSCREEN) - m_dlgSnapInfo.m_rect.Width() - MOUSE_SENSITIVITY
				&& mp.y >= GetSystemMetrics(SM_CYSCREEN) - m_dlgSnapInfo.m_rect.Height() - MOUSE_SENSITIVITY
			)
		{
			newX = 0;
			newY = 0;
		}
	}

	//is not capturing now, so we must consider the cross lines
	else
	{
		if ( mp.x <= m_dlgSnapInfo.m_rect.Width() + MOUSE_SENSITIVITY )
		{
			newX = GetSystemMetrics(SM_CXSCREEN) - m_dlgSnapInfo.m_rect.Width();

		}
		else if ( mp.x >= GetSystemMetrics(SM_CXSCREEN) - m_dlgSnapInfo.m_rect.Width() - MOUSE_SENSITIVITY )
		{
			newX = 0;
		}

		if ( mp.y <= m_dlgSnapInfo.m_rect.Height() + MOUSE_SENSITIVITY )
		{
			newY = GetSystemMetrics(SM_CYSCREEN) - m_dlgSnapInfo.m_rect.Height();
		}
		else if ( mp.y >= GetSystemMetrics(SM_CYSCREEN) - m_dlgSnapInfo.m_rect.Height() - MOUSE_SENSITIVITY )
		{
			newY = 0;
		}
	}


	m_dlgSnapInfo.SetColorInfo(clr);
	m_dlgSnapInfo.SetHintInfo(currentStatus);
	m_dlgSnapInfo.SetImageInfo(mp);
	m_dlgSnapInfo.SetWindowPos(&(CWnd::wndTopMost), newX, newY, 300, 300, 
							SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);

}

void CMySnapApp::HideSnapInfoDlg()
{
	m_dlgSnapInfo.ShowWindow(SW_HIDE);
}

void CAboutDlg::OnStnClickedHomepage()
{
	ShellExecute(NULL, "open", "http://www.zeroapp.com", NULL, NULL, SW_SHOWNORMAL);
}

BOOL CAboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CRect rect; 
	CPoint ptCursor; 
	CWnd *pStatic = GetDlgItem(IDC_HOMEPAGE); 
	pStatic->GetWindowRect(&rect); 
	GetCursorPos(&ptCursor); 
	if (rect.PtInRect (ptCursor)) 
	{ 
		CWinApp *pApp = AfxGetApp(); 
		HICON hIconBang=pApp->LoadCursor(IDC_PointToWeb); 
		SetCursor(hIconBang); 
		return TRUE; 
	} 
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

//------------ for register -------------------------------
CString CMySnapApp::GetRegCode()
{
	ASTAT Adapter;
	NCB ncb;
	UCHAR uRetCode;

	memset( &ncb, 0, sizeof(ncb) );
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = 0;

	uRetCode = Netbios( &ncb );
	memset( &ncb, 0, sizeof(ncb) );
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = 0;

	strcpy( (char*)(ncb.ncb_callname),  "*               " );
	ncb.ncb_buffer = (PUCHAR) &Adapter;
	ncb.ncb_length = sizeof(Adapter);

	uRetCode = Netbios( &ncb );
	if ( uRetCode == 0 )
	{
		CString strMAC;
		strMAC.Format("%d9%d6%d&1%d3%d6%d8",	Adapter.adapt.adapter_address[0] ^ 07,
												Adapter.adapt.adapter_address[1] ^ 07,
												Adapter.adapt.adapter_address[2] ^ 57,
												Adapter.adapt.adapter_address[3] ^ 67,
												Adapter.adapt.adapter_address[4] ^ 57,
												Adapter.adapt.adapter_address[5] ^ 97);
		return strMAC;
	}
	return NULL;
}

CString CMySnapApp::CalRegKey()
{

	return NULL;
}

BOOL CMySnapApp::CheckRegKey(CString key)
{
	return true;
}
//:~-------------------- end of register function ---------------------