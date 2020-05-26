#pragma once
#include<Windows.h>

//基底クラス 基本モジュールはTaskを継承する
class Task{
public:
	virtual void ScreenShot(HWND handle);
	virtual void CutOut();
	virtual void Initialize();
	virtual void Finalize();
	virtual void Updata();
	virtual void Draw();
	virtual void Calculation();
};