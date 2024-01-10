// TODO
// - SQLite
// - Stop on sleep?
// - Autostart / Installer (msix?)
// - Toast comfirmation notification on close


#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include "GUI.h"

LRESULT CALLBACK WindowProc(HWND windowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT:
		Paint(windowHandle);
		break;
	case WM_CLOSE: // When close button or alt-f4 is pressed
		DestroyWindow(windowHandle);
		break;
	case WM_DESTROY: // After main window is destoryed
		PostQuitMessage(0); // queues a WM_QUIT that, when handled by DefWindowProc will close the app
		break;
	case WM_SETCURSOR:
		return SetCursor(lParam);
	case WM_HOTKEY:
		// write to sqlite
		// close
		DestroyWindow(windowHandle);
		break;
	default:
		return DefWindowProc(windowHandle, uMsg, wParam, lParam);
	}
	return 0;
};

wchar_t* GetForegroundWindowName() {
	HWND foregroundWindow = GetForegroundWindow();
	const int MAX_NAME_LENGTH = 300;
	wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t) * MAX_NAME_LENGTH);
	GetWindowTextW(foregroundWindow, str, MAX_NAME_LENGTH);
	OutputDebugStringW(str);
	return str;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd = CreateTimerWindow(WindowProc, hInstance, nCmdShow);

	RegisterHotKey(hwnd, 999, MOD_ALT, 0x54); // alt + t

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); // does things with key messages
		DispatchMessage(&msg); // Calls into the WindowsProc we gave it in the WndClass
	}
	return 0;
}