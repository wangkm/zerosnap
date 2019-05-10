// MySnapView.h : CMySnapView 类的接口
//


#pragma once
#include "MySnapDoc.h"
#include "atltypes.h"
#include <math.h>


class CMySnapView : public CScrollView
{
protected: // 仅从序列化创建
	CMySnapView();
	DECLARE_DYNCREATE(CMySnapView)

// 属性
public:
	CMySnapDoc* GetDocument() const;

// 操作
public:

// 重写
	public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMySnapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
protected:

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	POINT m_startPoint;
	POINT m_endPoint;
	POINT m_oldPoint;	//used to erase line
	int   m_drawNum;
	BOOL  m_needErasePrevious;
	BOOL m_isCapturing;
	BOOL  m_withBorder;
	CPrintInfo m_prtInfo;
	void DrawCross(POINT point);
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	void PrintPageHeader(CDC* pDC, CPrintInfo* pInfo, const CString& strHeader);
	void ErasePreText(CDC* pdc, CPoint startPoint, CPoint oldPoint);
	void DrawSizeText(CDC* pdc, CPoint startPoint, CPoint point);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // MySnapView.cpp 的调试版本
inline CMySnapDoc* CMySnapView::GetDocument() const
   { return reinterpret_cast<CMySnapDoc*>(m_pDocument); }
#endif

