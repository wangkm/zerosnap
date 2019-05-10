#pragma once

class CScreenCamera
{
public:
	CScreenCamera(void);
	~CScreenCamera(void);

public:
	HBITMAP CaptureDesktop(PSIZE pImageSize);
	HBITMAP CaptureForegroundWindow(PSIZE pImageSize, BOOL bClientAreaOnly);
	HBITMAP CaptureWindow(HWND hWnd, PSIZE pImageSize, BOOL bClientAreaOnly);
	HBITMAP CaptureRegion(HDC srcDC, POINT startPoint, POINT endPoint, PSIZE pImageSize);

};
