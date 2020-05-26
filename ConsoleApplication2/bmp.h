#pragma once
#include<iostream>
//... http://hatenaclang.blogspot.jp/2011/04/blog-post_15.html
//... こちらのサイトを参考に汎用ヘッダーを設定

#ifdef WIN32
#include <windows.h>

#else

#include<stdint.h>
typedef uint8_t             BYTE;
#define BI_BITFIELDS        0x00000003

struct RGBQUAD {
		BYTE rgbBlue;
		BYTE rgbGreen;
		BYTE rgbRed;
		BYTE rgbReserved;
};

struct BITMAPFILEHEADER {
		uint16_t bfType;
		uint32_t bfSize;
		uint16_t bfReserved1;
		uint16_t bfReserved2;
		uint32_t bfOffBits;
};

struct BITMAPINFOHEADER {
		uint32_t biSize;
		uint32_t biWidth;
		uint32_t biHeight;
		uint16_t biPlanes;
		uint16_t biBitCount;
		uint32_t biCompression;
		uint32_t biSizeImage;
		uint32_t biXPelsPerMeter;
		uint32_t biYPelsPerMeter;
		uint32_t biClrUsed;
		uint32_t biClrImportant;
	
};

struct BITMAPINFO {
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD bmiColors[1];
};
#endif
class bmp {
	struct BMPInfo {
		uint32_t Width;
		int32_t  Height;
		uint32_t Line;
		uint8_t bCnt;
		uint8_t direction;
	};
public:
	static PBITMAPINFO LoadBITMAPINFO(uint32_t biWidth,int32_t biHeight,uint16_t biBitCount,uint32_t biCompression=0,uint32_t biClrUsed=0) {
		BITMAPINFO* info = new BITMAPINFO();
		info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info->bmiHeader.biWidth = biWidth;
		info->bmiHeader.biHeight = biHeight;
		info->bmiHeader.biPlanes = 1;
		info->bmiHeader.biBitCount = biBitCount;
		info->bmiHeader.biCompression = biCompression;
		info->bmiHeader.biSizeImage = 0;	//アテにならない
		info->bmiHeader.biXPelsPerMeter = 0;//意味なし
		info->bmiHeader.biYPelsPerMeter = 0;//意味なし
		info->bmiHeader.biClrUsed = biClrUsed;
		info->bmiHeader.biClrImportant = 0;	//意味なし

		return info;
	}
};