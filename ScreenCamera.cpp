#include "StdAfx.h"
#include ".\screencamera.h"
#include <math.h>
#include "MyDataDefine.h"
#include "MySnap.h"

CScreenCamera::CScreenCamera(void)
{
}

CScreenCamera::~CScreenCamera(void)
{
}

HBITMAP CScreenCamera::CaptureDesktop(PSIZE pImageSize)
{
    HWND hWnd = NULL;

    hWnd = GetDesktopWindow();          // Get handle to desktop window.

    return CaptureWindow(hWnd, pImageSize, FALSE);  // Capture an image of this window.
}

HBITMAP CScreenCamera::CaptureForegroundWindow(PSIZE pImageSize, BOOL bClientAreaOnly)
{
    HWND hWnd = NULL;

    hWnd = ::GetForegroundWindow();             // Get the foreground window.

    return CaptureWindow(hWnd, pImageSize, bClientAreaOnly);// Capture an image of this window.
}

HBITMAP CScreenCamera::CaptureWindow(HWND hWnd, PSIZE pImageSize, BOOL bClientAreaOnly)
{
    if(!hWnd)
        return NULL;

    HDC hdc;
    RECT rect;
    if(bClientAreaOnly)
    {
        hdc = GetDC(hWnd);
        GetClientRect(hWnd, &rect);
    }
    else
    {
        hdc = GetWindowDC(hWnd);
        GetWindowRect(hWnd, &rect);
    }

    if(!hdc)
        return NULL;

    HDC hMemDC = CreateCompatibleDC(hdc);
    if(hMemDC == NULL)
        return NULL;

    SIZE size;
    size.cx = rect.right - rect.left;
    if(rect.right < rect.left)
        size.cx = -size.cx;
    size.cy = rect.bottom - rect.top;
    if(rect.bottom < rect.top)
        size.cy = -size.cy;

    HBITMAP hDDBmp = CreateCompatibleBitmap(hdc, size.cx, size.cy);
    if(hDDBmp == NULL)
    {
        DeleteDC(hMemDC);
        ReleaseDC(hWnd, hdc);
        return NULL;
    }

    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hMemDC, hDDBmp));
    BitBlt(hMemDC, 0, 0, size.cx, size.cy, hdc, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hdc);

    HBITMAP hBmp = static_cast<HBITMAP>(CopyImage(hDDBmp,
                                                    IMAGE_BITMAP,
                                                    0,
                                                    0,
                                                    LR_COPYRETURNORG));

    DeleteObject(hDDBmp);

	pImageSize->cx = size.cx;
	pImageSize->cy = size.cy;

    return hBmp;
}

HBITMAP CScreenCamera::CaptureRegion(HDC srcDC, POINT startPoint, POINT endPoint, PSIZE pImageSize)
{
	SIZE size;
	size.cx = fabs(double(startPoint.x - endPoint.x));
	size.cy = fabs(double(startPoint.y - endPoint.y));
	int startX = startPoint.x < endPoint.x ? startPoint.x : endPoint.x;
	int startY = startPoint.y < endPoint.y ? startPoint.y : endPoint.y;
	
	HDC hMemDC = CreateCompatibleDC(srcDC);
	if(hMemDC == NULL)
	{
        return NULL;
	}

	ASSERT(size.cx >= 0 && size.cx <= 4096 && size.cy >= 0 && size.cy <= 4096);

	HBITMAP hDDBmp = CreateCompatibleBitmap(srcDC, size.cx, size.cy);
    if(hDDBmp == NULL)
    {
        DeleteDC(hMemDC);
        return NULL;
    }

    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hMemDC, hDDBmp));
	
    BitBlt(hMemDC, 
		0, 0, 
		size.cx, size.cy, 
		srcDC, 
		startX, startY, 
		SRCCOPY);

    HBITMAP hBmp = static_cast<HBITMAP>(CopyImage(hDDBmp,
                                                    IMAGE_BITMAP,
                                                    0,
                                                    0,
                                                    LR_COPYRETURNORG));

    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    DeleteObject(hDDBmp);

	pImageSize->cx = size.cx;
	pImageSize->cy = size.cy;

	return hBmp;
}
