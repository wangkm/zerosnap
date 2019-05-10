#pragma once


// CSnapInfoDlg dialog

class CSnapInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CSnapInfoDlg)

public:
	CSnapInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSnapInfoDlg();

// Dialog Data
	enum { IDD = IDD_SNAPINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	CRect m_rect;
	afx_msg void OnMove(int x, int y);

	void SetColorInfo(COLORREF clr);
	void SetHintInfo(int currentStatus);
	void SetImageInfo(CPoint pt);

	int oldStatus;
};
