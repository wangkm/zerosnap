// MySnapView.h : CMySnapView ��Ľӿ�
//


#pragma once
#include "MySnapDoc.h"
#include "atltypes.h"
#include <math.h>


class CMySnapView : public CScrollView
{
protected: // �������л�����
	CMySnapView();
	DECLARE_DYNCREATE(CMySnapView)

// ����
public:
	CMySnapDoc* GetDocument() const;

// ����
public:

// ��д
	public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CMySnapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // MySnapView.cpp �ĵ��԰汾
inline CMySnapDoc* CMySnapView::GetDocument() const
   { return reinterpret_cast<CMySnapDoc*>(m_pDocument); }
#endif

