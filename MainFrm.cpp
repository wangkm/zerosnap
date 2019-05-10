// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "MySnap.h"

#include "MainFrm.h"
#include ".\mainfrm.h"
#include <vfw.h>
#include "MyDataDefine.h"
#include "SnapInfoDlg.h"
#include "DlgResizeImg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_SNAP_SNAPREGION, OnSnapSnapregion)
	ON_WM_CLOSE()
	ON_COMMAND(ID_SNAP_ACTIVEWINDOW, OnSnapActivewindow)
	ON_COMMAND(ID_SNAP_SCREEN, OnSnapScreen)
	ON_COMMAND(ID_IMAGE_RESIZE, OnImageResize)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_RESIZE, OnUpdateImageResize)
	ON_COMMAND(ID_IMAGE_ROTATELEFT, OnImageRotateleft)
	ON_COMMAND(ID_IMAGE_ROTATERIGHT, OnImageRotateright)
	ON_COMMAND(ID_IMAGE_VERTICALFLIP, OnImageVerticalflip)
	ON_COMMAND(ID_IMAGE_HORIZONTALFLIP, OnImageHorizontalflip)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_HORIZONTALFLIP, OnUpdateImageHorizontalflip)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_VERTICALFLIP, OnUpdateImageVerticalflip)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_ROTATERIGHT, OnUpdateImageRotateright)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_ROTATELEFT, OnUpdateImageRotateleft)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_IMG_INFO,			// 当前图像的信息
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	m_isFullScreen = false;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//------------------ 创建256色 工具栏 --------------------------------------------
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	CImageList imageList;
	imageList.Create(32, 32, TRUE | ILC_COLOR8, 12, 0);
	HICON hIcon = NULL;

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_Open), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SAVE), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_Copy), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SnapRegion), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SnapDesktop), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SnapTopWindow), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_Resize), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_RotateL), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_RotateR), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FlipH), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FlipV), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_Help), IMAGE_ICON, 32, 32, 0);	
	imageList.Add(hIcon);	


	m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();


	//---------------------------------------------------------------------------------

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// 注册热键，by starry
	RegHotkey();

	return 0;
}

BOOL CMainFrame::RegHotkey()
{
	hotkeyID[0] = GlobalAddAtom("MySnap_snapRegion");
	hotkeyID[1] = GlobalAddAtom("MySnap_snapForegroundWindow");
	hotkeyID[2] = GlobalAddAtom("MySnap_snapDesktop");
	if (!RegisterHotKey(this->m_hWnd, hotkeyID[0], MOD_CONTROL | MOD_SHIFT, 0x52))	//"R"
	{
		AfxMessageBox("hotkey ctrl+shift+R has been regisered by other program.");
	}
	if (!RegisterHotKey(this->m_hWnd, hotkeyID[1], MOD_CONTROL | MOD_SHIFT, 0x46))	//"F"
	{
		AfxMessageBox("hotkey ctrl+shift+F has been regisered by other program.");
	}
	if (!RegisterHotKey(this->m_hWnd, hotkeyID[2], MOD_CONTROL | MOD_SHIFT, 0x44))	//"D"
	{
		AfxMessageBox("hotkey ctrl+shift+D has been regisered by other program.");
	}
	return TRUE;
}

BOOL CMainFrame::UnRegHotKey()
{
	UnregisterHotKey(this->m_hWnd, hotkeyID[0]);
	UnregisterHotKey(this->m_hWnd, hotkeyID[1]);
	UnregisterHotKey(this->m_hWnd, hotkeyID[2]);
	return TRUE;
}



// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序


BOOL CMainFrame::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	// unrigister hotkey
	UnRegHotKey();
	return CFrameWnd::DestroyWindow();
}

BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_HOTKEY)
	{
		if (wParam == hotkeyID[0])
		{
			OnSnapSnapregion();
		}
		else if (wParam == hotkeyID[1])
		{
			OnSnapActivewindow();
		}
		else if (wParam == hotkeyID[2])
		{
			OnSnapScreen();
		}
	}

	return CFrameWnd::OnWndMsg(message, wParam, lParam, pResult);
}


void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
//	if (m_isFullScreen && nState == WA_INACTIVE){
//		EndFullScreen();
//	}
}


BOOL CMainFrame::FreezeScreen()
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;

	// Redraw the application window. 
	GetWindowRect(&m_oldRect);

	ShowWindow(SW_MINIMIZE);
	while(!this->IsIconic())
	{
		// waite
	}
	ShowWindow(SW_HIDE);
	Sleep(SLEEP_TIME);
	


	////-------------------------
	//copy screen to compatible DC 
	//
	pDoc->ReleaseResource(FALSE);

	pDoc->m_hBitmap = GET_APP->m_screenCamera.CaptureDesktop(&(pDoc->imageSize));
	pDoc->m_hMemDC = CreateCompatibleDC(NULL);
	pDoc->m_hOldBmp = static_cast<HBITMAP>(SelectObject(pDoc->m_hMemDC, pDoc->m_hBitmap));

	pDoc->m_hasData = true;

	////---------------------------

	//---------- draw mouse cursor --------------------
	if(pDoc->m_withMouse){

		HICON hCur;
		POINT pt;
		GetCursorPos(&pt);
		CWnd *wnd = WindowFromPoint(pt);
		
		HWND hWindow = wnd->m_hWnd;
		
		DWORD dwCurrentThreadID, dwThreadID;

		if ( IsWindow(hWindow) )
		{
			// Get the thread ID for the cursor owner.
			dwThreadID = GetWindowThreadProcessId(hWindow, NULL);

			// Get the thread ID for the current thread
			dwCurrentThreadID = GetCurrentThreadId();

			// If the cursor owner is not us then we must attach to
			// the other thread in so that we can use GetCursor() to
			// return the correct hCursor
			if ( dwCurrentThreadID != dwThreadID )
			{
				if ( AttachThreadInput(dwCurrentThreadID, dwThreadID, true) )
				{
					// Get the handle to the cursor
					hCur = ::GetCursor();
					AttachThreadInput(dwCurrentThreadID, dwThreadID, false);
				}
			}
		}
		else
		{
			hCur = ::GetCursor();
		}

		// faint, to adjust, i don't know why.
		pt.x -= 10;
		pt.y -= 10;
		CDC* pDC = CDC::FromHandle(pDoc->m_hMemDC);
		pDC->DrawIcon(pt, hCur); 		

		pDC->Detach();

		//------------ end of draw mouse cursor ------------
	}


	//hide title bar
	LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
	style &= ~WS_CAPTION;
	::SetWindowLong(m_hWnd,GWL_STYLE,style);

	//store the system menu, and set current menu empty
	CMenu* pOldMenu = GetMenu();
	m_OrgMenu.Attach(pOldMenu->Detach());
	SetMenu((CMenu*)NULL);

	//Hide toolbar & statusbar
	m_wndToolBar.ShowWindow(SW_HIDE);
	m_wndStatusBar.ShowWindow(SW_HIDE);

	//set full screen sign to notify view to recalcute noneclient area
	m_isFullScreen = true;


	//Resize window
	SetWindowPos(&wndTopMost,
		0, 0, 
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		SWP_NOACTIVATE); 

	ShowWindow(SW_SHOWMAXIMIZED);

	return 0;
}



BOOL CMainFrame::EndFullScreen()
{
	if(!m_isFullScreen)
		return true;

	//Restore System Menu
	SetMenu(&m_OrgMenu);
	m_OrgMenu.Detach();
	
	//Restore window style
	LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
	style |= WS_CAPTION;
	::SetWindowLong(m_hWnd,GWL_STYLE,style);
	
	//Show toolbar & statusbar
	m_wndToolBar.ShowWindow(SW_SHOW);
	m_wndStatusBar.ShowWindow(SW_SHOW);

	SetWindowPos(&wndNoTopMost, m_oldRect.left, m_oldRect.top, m_oldRect.Width(), m_oldRect.Height(), SWP_HIDEWINDOW);
	ShowWindow(SW_NORMAL);

	m_isFullScreen = false;

	//------- hide snap info dialog ---------
	GET_APP->HideSnapInfoDlg();
	//:~----- end of hide snap info dialog ---


	return TRUE;
}


void CMainFrame::OnSnapSnapregion()
{
	// TODO: Add your command handler code here
	if (!m_isFullScreen)
	{
		FreezeScreen();
	}
}


void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	if ( pDoc->IsModified() ){
		int decide = AfxMessageBox("是否将当前图像保存到磁盘？", MB_YESNOCANCEL);
		switch(decide){
			case IDCANCEL:
				return;
			case IDYES:
				pDoc->OnFileSave();
				if( pDoc->IsModified() )
					return;
				break;
			case IDNO:
				pDoc->SetModifiedFlag(FALSE);
				break;
		}
	}
	CFrameWnd::OnClose();
}

void CMainFrame::OnSnapActivewindow()
{
	//hide myself first
	ShowWindow(SW_MINIMIZE);
	while(!this->IsIconic())
	{
		// waite
	}
	ShowWindow(SW_HIDE);
	Sleep(SLEEP_TIME);

	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;

	//release the previous resource
	pDoc->ReleaseResource(TRUE);

	//get handle of bitmap and dc from screen camera
	pDoc->m_hBitmap = GET_APP->m_screenCamera.CaptureForegroundWindow(&(pDoc->imageSize), false);
	pDoc->m_hMemDC = CreateCompatibleDC(NULL);
	pDoc->m_hOldBmp = static_cast<HBITMAP>(SelectObject(pDoc->m_hMemDC, pDoc->m_hBitmap));

	//--------- store data to image ----------------------------------
	//since CImage must own a bitmap itself, we make a copy of bitmap.
	HBITMAP hBmpTmp = static_cast<HBITMAP>(CopyImage(pDoc->m_hBitmap,
                                                IMAGE_BITMAP,
                                                0,
                                                0,
                                                LR_COPYRETURNORG));
	pDoc->image.Attach(hBmpTmp);

	//========= end of store ==========================================

	//modify the sign
	pDoc->m_hasData = true;

	//---------------- if there is a wave file, play it ---------
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char strWaveFileName[] = WAVE_ENDSNAP;
	hFind = FindFirstFile(strWaveFileName, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
		sndPlaySound(strWaveFileName, SND_ASYNC);
	//----------------- end of play wave ------------------------

	//show my self
	ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_NORMAL);

	//modify the flag
	pDoc->SetModifiedFlag(TRUE);
}

void CMainFrame::OnSnapScreen()
{
	//minimize and then hide myself first
	ShowWindow(SW_MINIMIZE);
	while(!this->IsIconic())
	{
		// waite
	}
	ShowWindow(SW_HIDE);
	Sleep(SLEEP_TIME);

	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;

	//release previous resource
	pDoc->ReleaseResource(TRUE);

	//get handle of bitmap and dc from screen camera
	pDoc->m_hBitmap = GET_APP->m_screenCamera.CaptureDesktop(&(pDoc->imageSize));
	pDoc->m_hMemDC = CreateCompatibleDC(NULL);
	pDoc->m_hOldBmp = static_cast<HBITMAP>(SelectObject(pDoc->m_hMemDC, pDoc->m_hBitmap));

	//--------- store data to image ----------------------------------
	//since CImage must own a bitmap itself, we make a copy of pDoc->m_hBitmap.
	HBITMAP hBmpTmp = static_cast<HBITMAP>(CopyImage(pDoc->m_hBitmap,
                                                IMAGE_BITMAP,
                                                0,
                                                0,
                                                LR_COPYRETURNORG));
	pDoc->image.Attach(hBmpTmp);

	//========= end of store ==========================================


	//modify the sign
	pDoc->m_hasData = true;

	//---------------- if there is a wave file, play it ---------
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char strWaveFileName[] = WAVE_ENDSNAP;
	hFind = FindFirstFile(strWaveFileName, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
		sndPlaySound(strWaveFileName, SND_ASYNC);
	//----------------- end of play wave ------------------------


	//show my self
	ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_NORMAL);

	//modify the flag
	pDoc->SetModifiedFlag(TRUE);

}

//Resize image
void CMainFrame::OnImageResize()
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;

	int oldWidth = pDoc->image.GetWidth();
	int oldHeight = pDoc->image.GetHeight();
	CDlgResizeImg dlgResizeImg(oldWidth, oldHeight, this);

	//user cancel
	if ( dlgResizeImg.DoModal() == IDCANCEL ){
		return;
	}

	int newWidth = dlgResizeImg.GetImgWidth();
	int newHeight = dlgResizeImg.GetImgHeight();
	
	ASSERT(newWidth > 0 && newHeight > 0);

	//no modification
	if ( newWidth == oldWidth && newHeight == oldHeight )
	{
		return;
	}

	//---------- start resize operation ----------------
	pDoc->ResizeImg(newWidth, newHeight);
	//:~---------- end of resize ------------------------

	this->GetActiveView()->Invalidate();

}

void CMainFrame::OnImageRotateleft()
{
	// TODO: Add your command handler code here
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	pDoc->RotateL();
	this->GetActiveView()->Invalidate();
}

void CMainFrame::OnImageRotateright()
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	pDoc->RotateR();
	this->GetActiveView()->Invalidate();
}

void CMainFrame::OnImageVerticalflip()
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	pDoc->FlipV();
	this->GetActiveView()->Invalidate();
}

void CMainFrame::OnImageHorizontalflip()
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	pDoc->FlipH();
	this->GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateImageResize(CCmdUI *pCmdUI)
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	if ( pDoc->m_hasData )
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnUpdateImageHorizontalflip(CCmdUI *pCmdUI)
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	if ( pDoc->m_hasData )
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnUpdateImageVerticalflip(CCmdUI *pCmdUI)
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	if ( pDoc->m_hasData )
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnUpdateImageRotateright(CCmdUI *pCmdUI)
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	if ( pDoc->m_hasData )
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnUpdateImageRotateleft(CCmdUI *pCmdUI)
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	if ( pDoc->m_hasData )
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnEditCopy()
{
	//copy to clipboard
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	if(pDoc->m_hasData)
	{
		pDoc->CopyImageToClipboard();
	}
}

void CMainFrame::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	CMySnapDoc* pDoc = ((CMySnapApp *)AfxGetApp())->m_pDoc;
	if ( pDoc->m_hasData )
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
