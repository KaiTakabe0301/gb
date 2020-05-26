#pragma once
#include<iostream>
#include<Windows.h>

int keyState[256] = { 0 };

int MygetKeyState() {
	for (int i = 0; i < 256; i++) {
		if (GetAsyncKeyState(i))
			keyState[i]++;
		else
			keyState[i] = 0;
	}
	return 0;
}