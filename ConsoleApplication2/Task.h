#pragma once
#include<Windows.h>

//���N���X ��{���W���[����Task���p������
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