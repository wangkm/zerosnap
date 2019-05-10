// MainFrm.h : CMainFrame ��Ľӿ�
//


#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:
	int hotkeyID[3];					//��¼�ȼ�ID
	BOOL  m_isFullScreen;				//ȫ����ʾ��־ 
//	BOOL  m_isSelectingWnd;				//is selecting window to capture

	CRect m_oldRect;					//����ԭʼλ��
	CMenu m_OrgMenu;					//ԭ�˵�
	BOOL  m_bChildMax;

// ����
public:

// ��д
public:

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	BOOL ShowFullScreen();
	BOOL FreezeScreen();
	BOOL EndFullScreen();
	BOOL RegHotkey();
	BOOL UnRegHotKey();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSnapSnapregion();
	afx_msg void OnClose();
	afx_msg void OnSnapActivewindow();
	afx_msg void OnSnapScreen();
	afx_msg void OnImageResize();
	afx_msg void OnUpdateImageResize(CCmdUI *pCmdUI);
	afx_msg void OnImageRotateleft();
	afx_msg void OnImageRotateright();
	afx_msg void OnImageVerticalflip();
	afx_msg void OnImageHorizontalflip();
	afx_msg void OnUpdateImageHorizontalflip(CCmdUI *pCmdUI);
	afx_msg void OnUpdateImageVerticalflip(CCmdUI *pCmdUI);
	afx_msg void OnUpdateImageRotateright(CCmdUI *pCmdUI);
	afx_msg void OnUpdateImageRotateleft(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	void SetImgInfoOnStatusBar(CString info)	{ m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_IMG_INFO),info); }
};


