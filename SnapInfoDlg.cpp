// SnapInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MySnap.h"
#include "SnapInfoDlg.h"
#include ".\snapinfodlg.h"
#include "MyDataDefine.h"


// CSnapInfoDlg dialog

IMPLEMENT_DYNAMIC(CSnapInfoDlg, CDialog)
CSnapInfoDlg::CSnapInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSnapInfoDlg::IDD, pParent)
{
}

CSnapInfoDlg::~CSnapInfoDlg()
{
}

void CSnapInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSnapInfoDlg, CDialog)
	ON_WM_PAINT()
//	ON_WM_MOUSEMOVE()
ON_WM_MOVE()
//ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CSnapInfoDlg message handlers

BOOL CSnapInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	SetWindowPos(&(CWnd::wndTopMost), 0, 0, 300, 300, 
			SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW | SWP_NOZORDER);
	GetWindowRect(&m_rect);
	//m_rect.left = 0;
	//m_rect.top = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSnapInfoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

void CSnapInfoDlg::SetColorInfo(COLORREF clr)
{
	int R = (int)(clr & 0x0000ff);
	int G = (int)((clr & 0x00ff00) >> 8);
	int B = (int)((clr & 0xff0000) >> 16);

	CString strTmp;
	strTmp.Format("R: %d", R);
	this->GetDlgItem(IDC_CLR_R)->SetWindowText(strTmp);
	strTmp.Format("G: %d", G);
	this->GetDlgItem(IDC_CLR_G)->SetWindowText(strTmp);
	strTmp.Format("B: %d", B);
	this->GetDlgItem(IDC_CLR_B)->SetWindowText(strTmp);

	this->UpdateData(false);

}


void CSnapInfoDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	GetWindowRect(&m_rect);

	// TODO: Add your message handler code here
}


void CSnapInfoDlg::SetHintInfo(int newStatus)
{
	if (oldStatus == newStatus)
	{
		return;
	}
	if (newStatus == STATUS_FREEMOVING)
	{
		GetDlgItem(IDC_LBTNInfo)->SetWindowText("左键：\t选择开始点");
		GetDlgItem(IDC_RBTNInfo)->SetWindowText("右键：\t取消抓图任务");
	}
	else if (newStatus == STATUS_CAPTURING)
	{
		GetDlgItem(IDC_LBTNInfo)->SetWindowText("左键：\t选择结束点");
		GetDlgItem(IDC_RBTNInfo)->SetWindowText("右键：\t重新选择开始点");
	}
	oldStatus = newStatus;
	UpdateData(false);

}

void CSnapInfoDlg::SetImageInfo(CPoint pt)
{
	int scrWidth  = GetSystemMetrics(SM_CXSCREEN);
	int scrHeight = GetSystemMetrics(SM_CYSCREEN);

	CMySnapDoc *pDoc = GET_APP->m_pView->GetDocument();	// +

	CMySnapView *pView = GET_APP->m_pView;
	CWnd *pWnd = this->GetDlgItem(IDC_IMAGEINFOBOX);
	CRect rect;
	pWnd->GetClientRect(&rect);
	CDC *pDesDC = pWnd->GetDC();
	CDC *pSrcDC = pView->GetDC();

	CPoint adjustPt;
	adjustPt.x = pt.x;
	adjustPt.y = pt.y;
	if (pt.x - ZOOMIN_REGION_SIZE / 2 < 0)
	{
		adjustPt.x = ZOOMIN_REGION_SIZE / 2;
	}
	if (pt.y - ZOOMIN_REGION_SIZE / 2 < 0)
	{
		adjustPt.y = ZOOMIN_REGION_SIZE / 2;
	}
	if (pt.x + ZOOMIN_REGION_SIZE / 2 > scrWidth)
	{
		adjustPt.x = scrWidth - ZOOMIN_REGION_SIZE / 2;
	}
	if (pt.y + ZOOMIN_REGION_SIZE / 2 > scrHeight)
	{
		adjustPt.y = scrHeight - ZOOMIN_REGION_SIZE / 2;
	}


	StretchBlt(pDesDC->m_hDC, 
			0, 0, 
			rect.Width(), rect.Height(), 
			pSrcDC->m_hDC,
			adjustPt.x - ZOOMIN_REGION_SIZE / 2, adjustPt.y - ZOOMIN_REGION_SIZE / 2,
			ZOOMIN_REGION_SIZE, ZOOMIN_REGION_SIZE,
			SRCCOPY);

	ReleaseDC(pSrcDC);
	ReleaseDC(pDesDC);

}

