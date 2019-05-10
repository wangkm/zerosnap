// MySnapView.cpp : CMySnapView 类的实现
//

#include "stdafx.h"
#include "MySnap.h"

#include "MySnapDoc.h"
#include "MySnapView.h"
#include ".\mysnapview.h"
#include "MainFrm.h"
#include "MyDataDefine.h"
#include "Mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMySnapView

IMPLEMENT_DYNCREATE(CMySnapView, CView)

BEGIN_MESSAGE_MAP(CMySnapView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

// CMySnapView 构造/析构

CMySnapView::CMySnapView()
{
	// TODO: 在此处添加构造代码
	((CMySnapApp *)AfxGetApp())->m_pView = this;
	m_isCapturing = false;
	m_needErasePrevious = false;
	m_withBorder = true;
	m_drawNum = 0;
}

CMySnapView::~CMySnapView()
{
}

BOOL CMySnapView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CView::PreCreateWindow(cs);
}

// CMySnapView 绘制

void CMySnapView::OnDraw(CDC* pDC)
{
	CMySnapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	if(!pDoc->m_hasData)
		return;

	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	CRect currtViewRect;	//for current scrollview, to display
	CRect srcRect;			//source rect, identify the position of memdc
	CRect desRect;			//destination rect, identify the rect to draw image

	GetWindowRect(&currtViewRect);

	//initial the drawing information
	if (pDC->IsPrinting())	//printer DC
	{
		// get printer pixels per inch
		int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
		int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

		//set desRect for printing
		desRect.top = m_prtInfo.m_rectDraw.top;
		desRect.left = m_prtInfo.m_rectDraw.left;

		//adjust the image print size, guarantee it can be printed in one page.
		desRect.right = desRect.left + (double)pDoc->image.GetWidth() * 0.01 * cxInch;
		if ( desRect.right > m_prtInfo.m_rectDraw.right )
		{
			desRect.right = m_prtInfo.m_rectDraw.right;
			desRect.bottom = desRect.top 
				+ (int)(((double)pDoc->image.GetHeight() * m_prtInfo.m_rectDraw.Width() * cyInch) 
				/ ((double)pDoc->image.GetWidth() * cxInch));
			if ( desRect.bottom > m_prtInfo.m_rectDraw.bottom )
			{
				desRect.bottom = m_prtInfo.m_rectDraw.bottom;
				desRect.right = desRect.left 
					+ (int)(((double)pDoc->image.GetWidth() * m_prtInfo.m_rectDraw.Height() * cxInch) 
					/ ((double)pDoc->image.GetHeight() * cyInch));
			}
		}
		else
		{
			desRect.bottom = desRect.top + (double)pDoc->image.GetHeight() * 0.01 * cyInch;
			if ( desRect.bottom > m_prtInfo.m_rectDraw.bottom )
			{
				desRect.bottom = m_prtInfo.m_rectDraw.bottom;
				desRect.right = desRect.left 
					+ (int)(((double)pDoc->image.GetWidth() * m_prtInfo.m_rectDraw.Height() * cxInch) 
					/ ((double)pDoc->image.GetHeight() * cyInch));
			}
		}

		//set srcRect, to print the whole image
		srcRect.top = srcRect.left = 0;
		srcRect.bottom = pDoc->image.GetHeight();
		srcRect.right = pDoc->image.GetWidth();

	}
	else	//not printer DC
	{
		if (pFrame->m_isFullScreen)	//in fullscreen
		{
			//set desRect, to show fullscreen;
			desRect.left = 0;
			desRect.top = 0;
			desRect.right = desRect.left + currtViewRect.Width();
			desRect.bottom = desRect.top + currtViewRect.Height();

			//set srcRect
			srcRect.left = 0;
			srcRect.top = 0;
			srcRect.right = desRect.Width();
			srcRect.bottom = desRect.Height();
		}
		else	//not in fullscreen
		{
			//get the current position of scrollview
			int x1 = GetScrollPos(SB_HORZ);
			int y1 = GetScrollPos(SB_VERT);

			//set desRect, to show fullscreen;
			desRect.left = x1 + ( m_withBorder? BORDER_WIDTH : 0 ) - 1;
			desRect.top = y1 + ( m_withBorder? BORDER_WIDTH : 0 ) - 1;
			desRect.right = desRect.left + currtViewRect.Width() + 1;
			desRect.bottom = desRect.top + currtViewRect.Height() + 1;

			//set srcRect
			srcRect.left = x1 - 1;
			srcRect.top = y1 - 1;
			srcRect.right = srcRect.left + desRect.Width();
			srcRect.bottom = srcRect.top + desRect.Height();
		}
	}

	//draw image
	//source rect and the destinate rect are the same size. Probably drawing on the screen.
	if ( srcRect.Width() == desRect.Width() 
		&& srcRect.Height() == desRect.Height() )	//the source rect and destinate rect have the same size
	{
		BitBlt(pDC->m_hDC, 
			desRect.left, desRect.top, 
			desRect.Width(), desRect.Height(), 
			pDoc->m_hMemDC,
			srcRect.left, srcRect.top, 
			SRCCOPY);
		
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		m_oldPoint = pt;
		
		//--------- draw previous lines or rectangle -----------

		if (pFrame->m_isFullScreen)
		{
			CPen pen; 
			pen.CreatePen(PS_SOLID, 1, SNAPLINE_COLOR); 
			CPen* oldPen = pDC->SelectObject(&pen); 
			pDC->SetROP2(R2_NOTXORPEN); 
			pDC->SetBkColor(TXTBG_COLOR);
			pDC->SetTextColor(TXT_COLOR);

			if (!m_isCapturing)
			{
				DrawCross(pt);
			}
			else
			{
				//draw selecting rectangle
				pDC->MoveTo(m_startPoint);
				pDC->LineTo(m_startPoint.x, pt.y);
				pDC->LineTo(pt);
				pDC->LineTo(pt.x, m_startPoint.y);
				pDC->LineTo(m_startPoint);

				//draw current selecting size text
				int newWidth  = pt.x - m_startPoint.x;
				int newHeight = pt.y - m_startPoint.y;
				CString strNewSize;
				strNewSize.Format("%d, %d", newWidth, newHeight);
				CSize sizeOfText = pDC->GetTextExtent(strNewSize);
				pDC->TextOut(m_startPoint.x + newWidth / 2 - sizeOfText.cx / 2, m_startPoint.y + newHeight / 2 - sizeOfText.cy / 2, strNewSize);
			}
			pDC->SelectObject(oldPen); 
			pen.DeleteObject();
			m_needErasePrevious = true;
		}
		//:~------------------- end of draw -------------------
	}
	//source rect and the destinate rect aren't the same size. Probably drawing to the printer.
	else
	{
		StretchBlt(pDC->m_hDC, 
			desRect.left, desRect.top, 
			desRect.Width(), desRect.Height(), 
			pDoc->m_hMemDC,
			srcRect.left, srcRect.top,
			srcRect.Width(), srcRect.Height(),
			SRCCOPY);

	}

	//if not in fullscreen and not for printing, draw a rectangle around the image
	if (!pDC->IsPrinting() && !pFrame->m_isFullScreen && m_withBorder)
	{
		if ( m_withBorder )
		{
			CPen newPen, *pOldPen;
			newPen.CreatePen(PS_SOLID, BORDER_WIDTH, BORDER_COLOR);
			pOldPen = pDC->SelectObject(&newPen);

			pDC->MoveTo(0, 0);
			pDC->LineTo(pDoc->imageSize.cx + BORDER_WIDTH, 0);
			pDC->LineTo(pDoc->imageSize.cx + BORDER_WIDTH, pDoc->imageSize.cy + BORDER_WIDTH);
			pDC->LineTo(0, pDoc->imageSize.cy + BORDER_WIDTH);
			pDC->LineTo(0, 0);

			pDC->SelectObject(pOldPen);
		}
		//设定状态栏中的图形信息
		CString strTmp;
		if(pDoc->m_hasData)
		{
			strTmp.Format("%d x %d", pDoc->image.GetWidth(), pDoc->image.GetHeight());
		}
		else
		{
			strTmp = "没有数据";
		}
		pFrame->SetImgInfoOnStatusBar(strTmp);
	}

	//in fullscreen, reset the scroll size to hide the scroll bar.
	if (pFrame->m_isFullScreen)
	{
		SIZE size;
		size.cx = 1;
		size.cy = 1;
		SetScrollSizes(MM_TEXT, size);
	}
	else
	{
		SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
	}

}


// CMySnapView 打印

BOOL CMySnapView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMySnapView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印前添加额外的初始化
}

void CMySnapView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印后添加清除过程
}


// CMySnapView 诊断

#ifdef _DEBUG
void CMySnapView::AssertValid() const
{
	CView::AssertValid();
}

void CMySnapView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMySnapDoc* CMySnapView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMySnapDoc)));
	return (CMySnapDoc*)m_pDocument;
}
#endif //_DEBUG


// CMySnapView 消息处理程序

void CMySnapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	//if(nChar == VK_ESCAPE){	//如果按的键为Esc键 
	//	//获取主框架窗口的指针 
	//	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	//	//退出全屏显示状态
	//	pFrame->EndFullScreen();
	//}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CMySnapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (pFrame->m_isFullScreen){
		//SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
		SetCursor(GET_APP->LoadCursor(IDC_CUR_SEL));
		return true;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CMySnapView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMySnapDoc* pDoc = GetDocument();
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	if ( !pFrame->m_isFullScreen)
		return;


	if ( !m_isCapturing ){
		m_startPoint = point;
		m_oldPoint = point;
		m_isCapturing = TRUE;
		if (m_needErasePrevious)
		{
			DrawCross(point);
		}
	}
	else{
		m_endPoint = point;
		m_isCapturing = FALSE;
		HBITMAP hBmpTmp;

		hBmpTmp = GET_APP->m_screenCamera.CaptureRegion(pDoc->m_hMemDC, m_startPoint, m_endPoint, &(pDoc->imageSize));

		//delete previous resourse, also delete the image
		pDoc->ReleaseResource(TRUE);
		CDC *pDC = GetDC();
		pDoc->m_hMemDC = CreateCompatibleDC(pDC->m_hDC);
		pDoc->m_hBitmap = hBmpTmp;
		pDoc->m_hOldBmp = static_cast<HBITMAP>(SelectObject(pDoc->m_hMemDC, pDoc->m_hBitmap));
		ReleaseDC(pDC);


		//--------- store data to image ----------
		//since CImage must own a bitmap itself, we make a copy of bitmap.
		hBmpTmp = static_cast<HBITMAP>(CopyImage(pDoc->m_hBitmap,
                                                    IMAGE_BITMAP,
                                                    0,
                                                    0,
                                                    LR_COPYRETURNORG));
		pDoc->image.Attach(hBmpTmp);

		//---------- end of store --------------

		pFrame->EndFullScreen();
		pDoc->m_hasData = true;

		//---------------- if there is a wave file, play it ---------
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		char strWaveFileName[] = WAVE_ENDSNAP;
		hFind = FindFirstFile(strWaveFileName, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
			sndPlaySound(strWaveFileName, SND_ASYNC);
		//:----------------- end of play wave ------------------------

		//modify the flag
		pDoc->SetModifiedFlag(TRUE);
	}

	m_needErasePrevious = false;

	CView::OnLButtonDown(nFlags, point);
}


void CMySnapView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMySnapDoc* pDoc = GetDocument();
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CDC* pDC = GetDC(); 

	if ( m_isCapturing ){
		m_isCapturing = false;
		m_needErasePrevious = false;

		CPen pen; 
		pen.CreatePen(PS_SOLID,1,RGB(0,0,0)); 
		CPen* oldPen=pDC->SelectObject(&pen); 
		pDC->SetROP2(R2_NOTXORPEN); 

		//erase previous lines and text
		ErasePreText(pDC, m_startPoint, m_oldPoint);

		pDC->SelectObject(oldPen); 
		pen.DeleteObject();


	}
	else{
		//detele previous resourse, but hold the image
		pDoc->ReleaseResource(FALSE);

		//restore the image into memDC
		if (!pDoc->image.IsNull())
		{
			pDoc->imageSize.cx = pDoc->image.GetWidth();
			pDoc->imageSize.cy = pDoc->image.GetHeight();
			pDoc->m_hMemDC = CreateCompatibleDC(pDC->m_hDC);
			pDoc->m_hBitmap = static_cast<HBITMAP>(CopyImage(
															pDoc->image,
															IMAGE_BITMAP,
															0,
															0,
															LR_COPYRETURNORG));
			pDoc->m_hOldBmp = static_cast<HBITMAP>(SelectObject(
															pDoc->m_hMemDC, 
															pDoc->m_hBitmap));
			pDoc->m_hasData = TRUE;
		}

		//return to normal window
		pFrame->EndFullScreen();
	}
	ReleaseDC(pDC);

	CView::OnRButtonDown(nFlags, point);
}

void CMySnapView::ErasePreText(CDC* pdc, CPoint startPoint, CPoint oldPoint)
{
	CSize sizeOfText;
	CString strOldSize;
	//erase selecting rectangle
	pdc->MoveTo(startPoint); 
	pdc->LineTo(startPoint.x, oldPoint.y);
	pdc->LineTo(oldPoint);
	pdc->LineTo(oldPoint.x, startPoint.y);
	pdc->LineTo(startPoint);

	//erase current selecting size text
	int oldWidth  = oldPoint.x - startPoint.x;
	int oldHeight = oldPoint.y - startPoint.y;
	strOldSize.Format("%d, %d", oldWidth, oldHeight);
	sizeOfText = pdc->GetTextExtent(strOldSize);
	int oldPosX, oldPosY;
	if ( abs(oldWidth) > DRAW_TXT_OUT_X && abs(oldHeight) > DRAW_TXT_OUT_Y )
	{
		//文字显示在区域内
		oldPosX = startPoint.x + oldWidth / 2 - sizeOfText.cx / 2;
		oldPosY = startPoint.y + oldHeight / 2 - sizeOfText.cy / 2;
	}
	else
	{
		//文字显示在区域外
		oldPosX = oldPoint.x > startPoint.x? oldPoint.x + 1: startPoint.x + 1;
		oldPosY = oldPoint.y > startPoint.y? oldPoint.y + 1: startPoint.y + 1;

		if (oldPosX + sizeOfText.cx > GetSystemMetrics(SM_CXSCREEN))
		{
			oldPosX = oldPoint.x > startPoint.x? oldPoint.x - sizeOfText.cx : startPoint.x - sizeOfText.cx;
		}
		if (oldPosY + sizeOfText.cy > GetSystemMetrics(SM_CYSCREEN))
		{
			oldPosY = oldPoint.y > startPoint.y? oldPoint.y - sizeOfText.cy : startPoint.y - sizeOfText.cy;
		}
	}
	CRect rect(	oldPosX, 
				oldPosY, 
				oldPosX + sizeOfText.cx, 
				oldPosY + sizeOfText.cy);
	CMySnapDoc* pDoc = GetDocument();
	BitBlt(pdc->m_hDC, 
		rect.left, rect.top,
		rect.Width(), rect.Height(), 
		pDoc->m_hMemDC, 
		rect.left, rect.top, 
		SRCCOPY);
}
void CMySnapView::DrawSizeText(CDC* pdc, CPoint startPoint, CPoint point)
{
	CSize sizeOfText;
	CString strNewSize;
	//draw selecting rectangle
	pdc->MoveTo(startPoint);
	pdc->LineTo(startPoint.x, point.y);
	pdc->LineTo(point);
	pdc->LineTo(point.x, startPoint.y);
	pdc->LineTo(startPoint);

	//draw current selecting size text
	int newWidth  = point.x - startPoint.x;
	int newHeight = point.y - startPoint.y;
	strNewSize.Format("%d, %d", newWidth, newHeight);
	sizeOfText = pdc->GetTextExtent(strNewSize);
	int newPosX, newPosY;
	if ( abs(newWidth) > DRAW_TXT_OUT_X && abs(newHeight) > DRAW_TXT_OUT_Y )
	{
		//文字显示在区域内
		newPosX = startPoint.x + newWidth / 2 - sizeOfText.cx / 2;
		newPosY = startPoint.y + newHeight / 2 - sizeOfText.cy / 2;
	}
	else
	{
		//文字显示在区域外
		newPosX = point.x > startPoint.x? point.x + 1 : startPoint.x + 1;
		newPosY = point.y > startPoint.y? point.y + 1 : startPoint.y + 1;

		if (newPosX + sizeOfText.cx > GetSystemMetrics(SM_CXSCREEN))
		{
			newPosX = point.x > startPoint.x? point.x - sizeOfText.cx : startPoint.x - sizeOfText.cx;
		}
		if (newPosY + sizeOfText.cy > GetSystemMetrics(SM_CYSCREEN))
		{
			newPosY = point.y > startPoint.y? point.y - sizeOfText.cy : startPoint.y - sizeOfText.cy;
		}
	}
	pdc->TextOut(newPosX, newPosY, strNewSize);

}
void CMySnapView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMySnapDoc* pDoc = GetDocument();
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (!pFrame->m_isFullScreen)
		return;
	
	if (m_oldPoint.x == point.x && m_oldPoint.y == point.y)
	{
		return;
	} 


	CDC* pdc = GetDC(); 

	if ( m_isCapturing )	//user has set the startpoint of region, now is set the endpoint
	{
		CPen pen; 
		pen.CreatePen(PS_SOLID, 1, SNAPLINE_COLOR); 
		CPen* oldPen=pdc->SelectObject(&pen); 
		pdc->SetROP2(R2_NOTXORPEN); 
		pdc->SetBkColor(TXTBG_COLOR);
		pdc->SetTextColor(TXT_COLOR);

		if ( m_needErasePrevious)
		{
			//erase previous line and text
			ErasePreText(pdc, m_startPoint, m_oldPoint);
		}

		//draw selecting rectangle and text
		DrawSizeText(pdc, m_startPoint, point);

		pdc->SelectObject(oldPen); 
		pen.DeleteObject();

		m_needErasePrevious = true;

		//show snap info dialog
		COLORREF clr = pdc->GetPixel(point);
		GET_APP->ShowSnapInfoDlg(point, clr, STATUS_CAPTURING);
	}
	else	// user is selecting the startpoint of the region
	{
		if (m_needErasePrevious)
		{
			DrawCross(m_oldPoint);
		}
		DrawCross(point);
		m_needErasePrevious = true;

		//show snap info dialog
		COLORREF clr = pdc->GetPixel(point);
		GET_APP->ShowSnapInfoDlg(point, clr, STATUS_FREEMOVING);
	}
	m_oldPoint = point;


	ReleaseDC(pdc);

}

//function to draw two cross line to facilitate user select point
void CMySnapView::DrawCross(POINT point)
{

	CMySnapDoc* pDoc = GetDocument();
	CDC* pdc = GetDC(); 
	CPen pen; 
	pen.CreatePen(PS_SOLID, 1, SNAPLINE_COLOR); 
	CPen* oldPen=pdc->SelectObject(&pen); 
	pdc->SetROP2(R2_NOTXORPEN);

	pdc->MoveTo(0, point.y); 
	pdc->LineTo(point.x, point.y);
	pdc->MoveTo(point.x, point.y);
	pdc->LineTo(GetSystemMetrics(SM_CXSCREEN), point.y);
	pdc->MoveTo(point.x, 0);
	pdc->LineTo(point.x, point.y);
	pdc->MoveTo(point.x, point.y);
	pdc->LineTo(point.x, GetSystemMetrics(SM_CYSCREEN));

	pdc->SelectObject(oldPen); 
	ReleaseDC(pdc);
	pen.DeleteObject();

}

BOOL CMySnapView::OnEraseBkgnd(CDC* pDC)
{
	// Set brush to desired background color 
	CBrush backBrush(BK_COLOR); 

	// Save old brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);

	CRect rect;
	pDC->GetClipBox(&rect); // Erase the area needed

	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);
	return TRUE;
}

void CMySnapView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
}

void CMySnapView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMySnapView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CMySnapView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	pInfo->m_rectDraw.left += PRN_BLK_LEFT;
	pInfo->m_rectDraw.right -= PRN_BLK_RIGHT;

	CTime currTime;
	currTime = CTime::GetCurrentTime();
	CString strTmp = GetDocument()->GetTitle();
	strTmp += " - ";
	strTmp += currTime.Format("[%Y.%m.%d %H:%M:%S]");
	PrintPageHeader(pDC, pInfo, strTmp);

	pInfo->m_rectDraw.bottom -= PRN_BLK_BOTTOM;

	//store print info into priviate attribute
	m_prtInfo.m_rectDraw = pInfo->m_rectDraw;

	CScrollView::OnPrint(pDC, pInfo);
}

void CMySnapView::PrintPageHeader(CDC* pDC, CPrintInfo* pInfo, const CString& strHeader)
{
	pDC->SetTextAlign(TA_LEFT);
	pDC->TextOut(pInfo->m_rectDraw.left, PRN_HEADER_POS, strHeader);	//1/4 inch down

	//Draw a line across the page, blow the header
	TEXTMETRIC textMetric;
	pDC->GetTextMetrics(&textMetric);
	int y = PRN_HEADER_POS + textMetric.tmHeight;


	pDC->MoveTo(pInfo->m_rectDraw.left, y);
	pDC->LineTo(pInfo->m_rectDraw.right, y);

	y += PRN_BLK_UND_HDLN;
	pInfo->m_rectDraw.top += y;

}


BOOL CMySnapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

//recalculate none client area to avoid a thin border when full screen
void CMySnapView::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (bCalcValidRects)
	{
		CMainFrame *pFrame = (CMainFrame*)GetParentFrame();
		if (pFrame && pFrame->m_isFullScreen)
		{
			return;
		}
	}

	CScrollView::OnNcCalcSize(bCalcValidRects, lpncsp);
}

LRESULT CMySnapView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	if (message == WM_PAINT)
	{
		//only invalidate the necessary areas
		int scrWidth  = GetSystemMetrics(SM_CXSCREEN);
		int scrHeight = GetSystemMetrics(SM_CYSCREEN);
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if (!m_isCapturing)
		{
			CRect rectH(0, pt.y - 1, scrWidth, pt.y + 1);
			CRect rectV(pt.x - 1, 0, pt.x + 1, scrHeight);
			this->InvalidateRect(rectH);
			this->InvalidateRect(rectV);
		}
		else
		{
			CRect rect(m_oldPoint, pt);
			InvalidateRect(rect);
		}
	}
	return CScrollView::WindowProc(message, wParam, lParam);
}

