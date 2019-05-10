#pragma once


// CDlgResizeImg dialog

class CDlgResizeImg : public CDialog
{
	DECLARE_DYNAMIC(CDlgResizeImg)

public:
	CDlgResizeImg(int oldWidth, int oldHeight, CWnd* pParent /*=NULL*/);   // standard constructor
	virtual ~CDlgResizeImg();

// Dialog Data
	enum { IDD = IDD_SetImgSize };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int imgWidth;
	int imgHeight;
	BOOL keepRatio;

private:
	int orgWidth;
	int orgHeight;

public:
	int GetImgWidth()	{return imgWidth;}
	int GetImgHeight()	{return imgHeight;}
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnChangeEdit2();
};
