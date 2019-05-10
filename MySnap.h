// MySnap.h : MySnap Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������
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
// �йش����ʵ�֣������ MySnap.cpp
//

class CMySnapApp : public CWinApp
{

public:
	CMySnapApp();
	~CMySnapApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
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
