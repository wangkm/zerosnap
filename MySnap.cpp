// MySnap.cpp : ����Ӧ�ó��������Ϊ��
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
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMySnapApp ����

CMySnapApp::CMySnapApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
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
// Ψһ��һ�� CMySnapApp ����

CMySnapApp theApp;

// CMySnapApp ��ʼ��

BOOL CMySnapApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
//	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
//	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMySnapDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CMySnapView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	//create snapinfo dialog
	m_dlgSnapInfo.Create(IDD_SNAPINFO);
//	m_dlgSnapInfo.ShowWindow(SW_HIDE);

	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������ں�׺ʱ�ŵ��� DragAcceptFiles��
	// �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand  ֮����



	return TRUE;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CMySnapApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMySnapApp ��Ϣ�������


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