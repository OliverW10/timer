#pragma once
#include <Windows.h>

HWND CreateTimerWindow(WNDPROC windowProc, HINSTANCE hInstance, int nCmdShow);

void Paint(HWND windowHandle);

bool SetCursor(LPARAM lParam);
