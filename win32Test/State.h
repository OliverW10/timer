#pragma once

#include "Windows.h"

typedef struct {
	wchar_t* appName;
	double elapsed;
} AppState;

inline AppState* GetAppState(HWND hwnd)
{
	LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	AppState* pState = reinterpret_cast<AppState*>(ptr);
	return pState;
}