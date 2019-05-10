// MySnapDoc.cpp :  CMySnapDoc 类的实现
//

#include "stdafx.h"
#include "MySnap.h"

#include "MySnapDoc.h"
#include ".\mysnapdoc.h"
#include "MyDataDefine.h"
#include <shlwapi.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMySnapDoc

IMPLEMENT_DYNCREATE(CMySnapDoc, CDocument)

BEGIN_MESSAGE_MAP(CMySnapDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()


char* CMySnapDoc::szFiltersForOpen = "\
图形文件|*.BMP;*.DIB;*.RLE;*.JPG;*.JPEG;*.JPE;*.JFIF;*.GIF;*.PNG;*.TIF;*.TIFF|\
JPEG(*.JPG;*.JPEG;*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;*.JFIF|\
GIF(*.GIF)|*.GIF|\
PNG (*.PNG)|*.PNG|\
TIFF(*.TIF;*.TIFF)|*.TIF;*.TIFF|\
BMP(*.BMP;*.DIB;*.RLE)|*.BMP;*.DIB;*.RLE||";

char* CMySnapDoc::szFiltersForSave = "\
BMP files(*.BMP)|*.BMP|\
JPEG files (*.JPG)|*.JPG|\
GIF files (*.GIF)|*.GIF|\
PNG files (*.PNG)|*.PNG|\
TIFF(*.TIF)|*.TIF||";

// CMySnapDoc 构造/析构

CMySnapDoc::CMySnapDoc()
{
	// TODO: 在此添加一次性构造代码
	((CMySnapApp *)AfxGetApp())->m_pDoc = this;
	m_hasData = false;
	m_withMouse = true;

	//set hogvideo
	if ( CAP_VIDEO_SWITCH )
	{
		CString strVideoTmpFile = GET_APP->m_strAppPath + "\\" + CAP_VIDEO_FILE;
		m_objHog.SetVideo(strVideoTmpFile);
		m_objHog.Hog();
	}

}

CMySnapDoc::~CMySnapDoc()
{
	ReleaseResource();
}

// CMySnapDoc 序列化

void CMySnapDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


void CMySnapDoc::CopyImageToClipboard()
{

	if ( !m_hasData )
	{
		return;
	}

	if ( !OpenClipboard(GET_APP->m_pMainWnd->GetSafeHwnd()) )
	{
		AfxMessageBox("error in open clipboard");
		return;
	}

	EmptyClipboard();

	SetClipboardData(CF_BITMAP, m_hBitmap);

	CloseClipboard() ;
}



//----------------------------------------------------------------------
// CMySnapDoc 诊断

#ifdef _DEBUG
void CMySnapDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMySnapDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMySnapDoc 命令


void CMySnapDoc::OnFileSave()
{
	if (!IsModified())
		return;
	CFileDialog dlgFile(FALSE, "bmp", "*.bmp", OFN_OVERWRITEPROMPT, szFiltersForSave);
	if(dlgFile.DoModal() == IDCANCEL)
		return;

	OnSaveDocument(dlgFile.GetPathName());

}

void CMySnapDoc::OnFileSaveAs()
{
	CFileDialog dlgFile(FALSE, "bmp", this->m_strPathName, OFN_OVERWRITEPROMPT, szFiltersForSave);
	if(dlgFile.DoModal() == IDCANCEL)
		return;

	OnSaveDocument(dlgFile.GetPathName());

}

SIZE CMySnapDoc::GetDocSize()
{
	SIZE sizeTotal;
	sizeTotal.cx = imageSize.cx > 0 ? imageSize.cx + BORDER_WIDTH + 1 : 1;
	sizeTotal.cy = imageSize.cy > 0 ? imageSize.cy + BORDER_WIDTH + 1 : 1;
	return sizeTotal;
}

BOOL CMySnapDoc::OnSaveDocument(LPCTSTR lpszPathName)
{

	// the extension on the file name will determine the file type that is saved
	HRESULT hResult;
	hResult = image.Save(_T(lpszPathName));
	if (FAILED(hResult)) {
		CString fmt;
		fmt.Format("Save image failed:\n%x - %s", hResult, _com_error(hResult).ErrorMessage());
		::AfxMessageBox(fmt);
		return FALSE;
	}

	this->m_strPathName = lpszPathName;
	this->SetModifiedFlag(FALSE);
	this->SetTitle(lpszPathName);
	return TRUE;

}

void CMySnapDoc::OnFileOpen()
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, szFiltersForOpen);
	if(dlgFile.DoModal() == IDCANCEL)
		return;

	OnOpenDocument(dlgFile.GetPathName());
}

BOOL CMySnapDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  Add your specialized creation code here
	ReleaseResource();

	image.Load(_T(lpszPathName));

	imageSize.cx = image.GetWidth();
	imageSize.cy = image.GetHeight();

	CDC* pDC = GET_APP->m_pView->GetDC();

	m_hMemDC = CreateCompatibleDC(NULL);


	m_hBitmap = static_cast<HBITMAP>(CopyImage((HBITMAP)image ,
                                               IMAGE_BITMAP,
                                               0,
                                               0,
                                               LR_CREATEDIBSECTION));

	m_hOldBmp = static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hBitmap));

	this->m_strPathName = lpszPathName;
	this->SetModifiedFlag(FALSE);
	this->SetTitle(lpszPathName);

	m_hasData = true;
	UpdateAllViews(NULL);

	return TRUE;
}


void CMySnapDoc::ReleaseResource(BOOL bDeleteImg)
{
	if(!m_hasData)
		return;
	try
	{
		SelectObject(m_hMemDC, m_hOldBmp);
		DeleteDC(m_hMemDC);
		DeleteObject(m_hBitmap);
		if (bDeleteImg)
		{
			image.Destroy();
		}
		m_hasData = false;
		this->SetModifiedFlag(FALSE);
	}
	catch(...)
	{
		AfxMessageBox("release resource error");
	}
}

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
		AfxMessageBox("GetObject error");

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 

     if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for the 24-bit-per-pixel format. 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98/Me, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
 } 


BOOL CMySnapDoc::ResizeImg(int newWidth, int newHeight)
{
	BITMAPINFO *pBitMapInfo;
	pBitMapInfo = CreateBitmapInfoStruct(this->m_hBitmap);
	ULONG  bfoffBits = 0;

	//store bmp to memory
	LPBYTE lpData = new BYTE[pBitMapInfo->bmiHeader.biSizeImage];
	int nReturnLine = GetDIBits(m_hMemDC, m_hBitmap, 1, imageSize.cy, lpData, pBitMapInfo, DIB_RGB_COLORS);
	if (nReturnLine != imageSize.cy - 1)
	{
		AfxMessageBox("error on resize");
		return FALSE;
	}

	//release previous data
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hBitmap);
	CDC* pDC = GET_APP->m_pView->GetDC();
	m_hBitmap = CreateCompatibleBitmap(pDC->m_hDC, newWidth, newHeight);
	GET_APP->m_pView->ReleaseDC(pDC);
	m_hOldBmp = static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hBitmap));


	//use drawDIB
	DrawDibRealize( GET_APP->m_hDrawDib, m_hMemDC, TRUE);

	DrawDibDraw( GET_APP->m_hDrawDib, m_hMemDC,
			0, 0, newWidth, newHeight,
			&(pBitMapInfo->bmiHeader), (LPVOID)lpData,
			0, 0, imageSize.cx, imageSize.cy,
			DDF_BACKGROUNDPAL);

	imageSize.cx = newWidth;
	imageSize.cy = newHeight;

	//update image
	image.Destroy();
	HBITMAP hBmpTmp = static_cast<HBITMAP>(CopyImage(m_hBitmap,
                                                IMAGE_BITMAP,
                                                0,
                                                0,
                                                LR_COPYRETURNORG));
	image.Attach(hBmpTmp);

	//release resource
	LocalFree(pBitMapInfo);
	delete lpData;

	return TRUE;
}

BOOL CMySnapDoc::RotateL(int angle)
{
	HDC hdcTmp;
	HBITMAP hBitmapTmp, hOldBmp;

	hdcTmp = CreateCompatibleDC(NULL);
	CDC* pDC = GET_APP->m_pView->GetDC();
	hBitmapTmp = CreateCompatibleBitmap(pDC->m_hDC, imageSize.cy, imageSize.cx);
	GET_APP->m_pView->ReleaseDC(pDC);
	hOldBmp = static_cast<HBITMAP>(SelectObject(hdcTmp, hBitmapTmp));

	POINT pt[3];
	pt[0].x = 0;
	pt[0].y = imageSize.cx;
	pt[1].x = 0;
	pt[1].y = 0;
	pt[2].x = imageSize.cy;
	pt[2].y = imageSize.cx;
	PlgBlt(hdcTmp, pt, m_hMemDC, 0, 0, imageSize.cx, imageSize.cy, NULL, 0, 0);

	//release previous data
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hMemDC);
	DeleteObject(m_hBitmap);

	m_hMemDC = hdcTmp;
	m_hBitmap = hBitmapTmp;
	m_hOldBmp = hOldBmp;

	int temp = imageSize.cx;
	imageSize.cx = imageSize.cy;
	imageSize.cy = temp;


	//update image
	image.Destroy();
	HBITMAP hBmpTmp = static_cast<HBITMAP>(CopyImage(m_hBitmap,
                                                IMAGE_BITMAP,
                                                0,
                                                0,
                                                LR_COPYRETURNORG));
	image.Attach(hBmpTmp);

	return TRUE;
}

BOOL CMySnapDoc::RotateR(int angle)
{
	HDC hdcTmp;
	HBITMAP hBitmapTmp, hOldBmp;

	hdcTmp = CreateCompatibleDC(NULL);
	CDC* pDC = GET_APP->m_pView->GetDC();
	hBitmapTmp = CreateCompatibleBitmap(pDC->m_hDC, imageSize.cy, imageSize.cx);
	GET_APP->m_pView->ReleaseDC(pDC);
	hOldBmp = static_cast<HBITMAP>(SelectObject(hdcTmp, hBitmapTmp));

	POINT pt[3];
	pt[0].x = imageSize.cy;
	pt[0].y = 0;
	pt[1].x = imageSize.cy;
	pt[1].y = imageSize.cx;
	pt[2].x = 0;
	pt[2].y = 0;
	PlgBlt(hdcTmp, pt, m_hMemDC, 0, 0, imageSize.cx, imageSize.cy, NULL, 0, 0);

	//release previous data
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hMemDC);
	DeleteObject(m_hBitmap);

	m_hMemDC = hdcTmp;
	m_hBitmap = hBitmapTmp;
	m_hOldBmp = hOldBmp;

	int temp = imageSize.cx;
	imageSize.cx = imageSize.cy;
	imageSize.cy = temp;


	//update image
	image.Destroy();
	HBITMAP hBmpTmp = static_cast<HBITMAP>(CopyImage(m_hBitmap,
                                                IMAGE_BITMAP,
                                                0,
                                                0,
                                                LR_COPYRETURNORG));
	image.Attach(hBmpTmp);

	return TRUE;
}

BOOL CMySnapDoc::FlipH()
{
	HDC hdcTmp;
	HBITMAP hBitmapTmp, hOldBmp;

	hdcTmp = CreateCompatibleDC(NULL);
	CDC* pDC = GET_APP->m_pView->GetDC();
	hBitmapTmp = CreateCompatibleBitmap(pDC->m_hDC, imageSize.cx, imageSize.cy);
	GET_APP->m_pView->ReleaseDC(pDC);
	hOldBmp = static_cast<HBITMAP>(SelectObject(hdcTmp, hBitmapTmp));

	POINT pt[3];
	pt[0].x = imageSize.cx;
	pt[0].y = 0;
	pt[1].x = 0;
	pt[1].y = 0;
	pt[2].x = imageSize.cx;
	pt[2].y = imageSize.cy;
	PlgBlt(hdcTmp, pt, m_hMemDC, 0, 0, imageSize.cx, imageSize.cy, NULL, 0, 0);

	//release previous data
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hMemDC);
	DeleteObject(m_hBitmap);

	m_hMemDC = hdcTmp;
	m_hBitmap = hBitmapTmp;
	m_hOldBmp = hOldBmp;

	//update image
	image.Destroy();
	HBITMAP hBmpTmp = static_cast<HBITMAP>(CopyImage(m_hBitmap,
                                                IMAGE_BITMAP,
                                                0,
                                                0,
                                                LR_COPYRETURNORG));
	image.Attach(hBmpTmp);

	return TRUE;
}
BOOL CMySnapDoc::FlipV()
{
	HDC hdcTmp;
	HBITMAP hBitmapTmp, hOldBmp;

	hdcTmp = CreateCompatibleDC(NULL);
	CDC* pDC = GET_APP->m_pView->GetDC();
	hBitmapTmp = CreateCompatibleBitmap(pDC->m_hDC, imageSize.cx, imageSize.cy);
	GET_APP->m_pView->ReleaseDC(pDC);
	hOldBmp = static_cast<HBITMAP>(SelectObject(hdcTmp, hBitmapTmp));

	POINT pt[3];
	pt[0].x = 0;
	pt[0].y = imageSize.cy;
	pt[1].x = imageSize.cx;
	pt[1].y = imageSize.cy;
	pt[2].x = 0;
	pt[2].y = 0;
	PlgBlt(hdcTmp, pt, m_hMemDC, 0, 0, imageSize.cx, imageSize.cy, NULL, 0, 0);

	//release previous data
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hMemDC);
	DeleteObject(m_hBitmap);

	m_hMemDC = hdcTmp;
	m_hBitmap = hBitmapTmp;
	m_hOldBmp = hOldBmp;

	//update image
	image.Destroy();
	HBITMAP hBmpTmp = static_cast<HBITMAP>(CopyImage(m_hBitmap,
                                                IMAGE_BITMAP,
                                                0,
                                                0,
                                                LR_COPYRETURNORG));
	image.Attach(hBmpTmp);

	return TRUE;
}
