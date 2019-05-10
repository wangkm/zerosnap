// MainFrm.h : CMainFrame 类的接口
//


#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:
	int hotkeyID[3];					//记录热键ID
	BOOL  m_isFullScreen;				//全屏显示标志 
//	BOOL  m_isSelectingWnd;				//is selecting window to capture

	CRect m_oldRect;					//窗口原始位置
	CMenu m_OrgMenu;					//原菜单
	BOOL  m_bChildMax;

// 操作
public:

// 重写
public:

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// 生成的消息映射函数
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


