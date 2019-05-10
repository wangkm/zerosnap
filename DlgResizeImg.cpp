// DlgResizeImg.cpp : implementation file
//

#include "stdafx.h"
#include "MySnap.h"
#include "DlgResizeImg.h"
#include ".\dlgresizeimg.h"


// CDlgResizeImg dialog

IMPLEMENT_DYNAMIC(CDlgResizeImg, CDialog)
CDlgResizeImg::CDlgResizeImg(int oldWidth, int oldHeight, CWnd* pParent /*=NULL*/)	//(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgResizeImg::IDD, pParent)
{
	orgWidth  = imgWidth  = oldWidth;
	orgHeight = imgHeight = oldHeight;
	keepRatio = TRUE;
}

CDlgResizeImg::~CDlgResizeImg()
{
}

void CDlgResizeImg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, imgWidth);
	DDX_Text(pDX, IDC_EDIT2, imgHeight);
	DDX_Check(pDX, IDC_CHECK1, keepRatio);
	DDV_MinMaxInt(pDX, imgWidth, 1, 4096);
	DDV_MinMaxInt(pDX, imgHeight, 1, 4096);
}


BEGIN_MESSAGE_MAP(CDlgResizeImg, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
END_MESSAGE_MAP()


// CDlgResizeImg message handlers

void CDlgResizeImg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if ( keepRatio )
	{
		UpdateData(TRUE);
		imgHeight = (double)imgWidth * ((double)orgHeight / (double)orgWidth);
		if ( imgHeight < 1 )
		{
			imgHeight = 1;
		}
		UpdateData(FALSE);
	}

}

void CDlgResizeImg::OnBnClickedCheck1()
{
	UpdateData(TRUE);
}

void CDlgResizeImg::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if ( keepRatio )
	{
		UpdateData(TRUE);
		double ratio = (double)orgHeight / (double)orgWidth;
		imgWidth = (double)imgHeight * ((double)orgWidth / (double)orgHeight);
		if ( imgWidth < 1 )
		{
			imgWidth = 1;
		}
		UpdateData(FALSE);
	}
}

