// MySnap.h : MySnap 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号
#include "MySnapView.h"
#include "ScreenCamera.h"
#include "SnapInfoDlg.h"
#include <nb30.h>
#include <Vfw.h>


typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

// CMySnapApp:
// 有关此类的实现，请参阅 MySnap.cpp
//

class CMySnapApp : public CWinApp
{

public:
	CMySnapApp();
	~CMySnapApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

public:
	CString m_strAppPath;
	CMySnapView *m_pView;
	CMySnapDoc  *m_pDoc;
	CScreenCamera m_screenCamera;
	CSnapInfoDlg m_dlgSnapInfo;
	HDRAWDIB  m_hDrawDib;  //to use drawdib

	void ShowSnapInfoDlg(CPoint mp, COLORREF clr, int currentStatus);
	void HideSnapInfoDlg();
	CString GetRegCode();
	CString CalRegKey();
	BOOL CheckRegKey(CString key);
};

extern CMySnapApp theApp;
