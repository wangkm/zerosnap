// MySnapDoc.h :  CMySnapDoc 类的接口
//


#pragma once
#include <atlimage.h>
#include <comdef.h>
#include "HogVideo.h"

class CMySnapDoc : public CDocument
{
protected: // 仅从序列化创建
	CMySnapDoc();
	DECLARE_DYNCREATE(CMySnapDoc)

// 属性
public:

// 操作
public:

// 重写
	public:
//	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CMySnapDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

// added by starry
public:
	BOOL	m_hasData;
	HDC		m_hMemDC;
	HBITMAP m_hOldBmp;
	HBITMAP m_hBitmap;
	SIZE	imageSize;
	CImage	image;
	BOOL	m_withMouse;
	//for capture video
	CHogVideo m_objHog;
	//:~
	static char* szFiltersForOpen;
	static char* szFiltersForSave;

	void CopyImageToClipboard();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	SIZE GetDocSize();
	void ReleaseResource(BOOL bDeleteImg = TRUE);

	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	BOOL ResizeImg(int newWidth, int newHeight);
	BOOL RotateL(int angle = 90);
	BOOL RotateR(int angle = 90);
	BOOL FlipH();
	BOOL FlipV();
};


