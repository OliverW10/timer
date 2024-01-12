#pragma once
#include <Windows.h>
#include "State.h"

HWND CreateTimerWindow(WNDPROC windowProc, HINSTANCE hInstance, int nCmdShow, AppState* appState);

void Paint(HWND windowHandle);

bool SetCursor(LPARAM lParam);
