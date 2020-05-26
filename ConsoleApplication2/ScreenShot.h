#pragma once
#include"bmp.h"
#include<Windows.h>

class ScreenShot {
private:
	HWND window;
	RECT rc;
	HDC hdc, hMemDC;

	PBITMAPINFO bmpInfo;
	PBITMAP bmBitmap;
	HBITMAP hBitmap;
	LPDWORD lpPixel;

public:
	ScreenShot() {
		bmBitmap = new BITMAP();
		bmpInfo = bmp::LoadBITMAPINFO(0, 0, 24, BI_RGB);
	}
	ScreenShot(HWND handle) {
		window = handle;
		ScreenShot();
	}
	bool SelectHWND(HWND handle) {
		window = handle;

		if (window != NULL)
			return true;
		else
			return false;
	}

//cv::Mat と IplImageが使用可能なら
#if defined(__OPENCV_CORE_MAT_HPP__) && defined(__OPENCV_CORE_TYPES_H__)
	bool Capture(IplImage* img) {
		PBITMAP tempBitmap = Capture();
		if (tempBitmap->bmBits == NULL)
			return false;
		memcpy(img->imageData, tempBitmap->bmBits, tempBitmap->bmWidthBytes*tempBitmap->bmHeight);
		Release();
		return true;
	}
//cv::Matのみ利用可能なら
#elif defined(__OPENCV_CORE_MAT_HPP__)

//IplImageのみ利用可能なら
#elif #defined(__OPENCV_CORE_MAT_HPP__)

#endif
	PBITMAP Capture() {
		GetWindowRect(window, &rc);

		bmpInfo->bmiHeader.biHeight = -(rc.bottom - rc.top);
		bmpInfo->bmiHeader.biWidth = rc.right - rc.left;

		hdc = GetDC(window);
		hMemDC = CreateCompatibleDC(hdc);
		hBitmap= CreateDIBSection(hdc, bmpInfo, DIB_RGB_COLORS, (void**)&lpPixel, NULL, 0);
		SelectObject(hMemDC, hBitmap);//ブラシとかそういうのみたいに選択する
		BitBlt(hMemDC, 0, 0, bmpInfo->bmiHeader.biWidth, -(bmpInfo->bmiHeader.biHeight), hdc, 0, 0, SRCCOPY);//ビットブロック転送
		GetObject(hBitmap, sizeof(BITMAP), bmBitmap);
		
		return bmBitmap;
	}

	void Release() {
		ReleaseDC(window, hdc);
		DeleteDC(hMemDC);
		DeleteObject(hBitmap);
	}
};