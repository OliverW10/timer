// TODO
// - No window, just text
// - Show in system tray
// - Shortcuts
// - SQLite
// - Stop on sleep?
// - Autostart / Installer


#include <windows.h>
#include <stdio.h>
#include <strsafe.h>
#include <stdint.h>

#ifndef UNICODE
#define UNICODE
#endif 

const int wordMask = (1 << 16) - 1;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_SIZE:
		int width = lParam & wordMask;
		int height = (lParam >> 16) & wordMask;
		wchar_t* msg;
		StringCbPrintfW(msg, 32, L"Resized: %d, %d\n", width, height);
		OutputDebugStringW(msg);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		Paint(ps.rcPaint);
		EndPaint(hwnd, &ps);
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
};

void Paint(HDC hdc, RECT region) {
	FillRect(hdc, &region, (HBRUSH)(COLOR_WINDOW + 1));
}

WNDCLASSEX CreateMainWindowClass(HINSTANCE applicationInstanceHandle, const wchar_t* className) {
	WNDCLASSEX wc = {}; // brackets are to zero the memory i think?
	// using the EX version because I will likely need to set the small icon (for when in tool bar)
	// lpfnWndProc = long pointer to a function called Window Procedure (i love microsoft <3)
	wc.lpfnWndProc = WindowProc; // will write this later
	wc.hInstance = applicationInstanceHandle;
	wc.lpszClassName = className;
	wc.style = CS_BYTEALIGNCLIENT | CS_DROPSHADOW;
	// CS_NOCLOSE
	return wc;
}


const wchar_t wndClassName[] = L"IDontKnowWhatThisShouldBe";
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	// Create a console to debug with. https://stackoverflow.com/questions/1333527/how-do-i-print-to-the-debug-output-window-in-a-win32-app
	/*AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);*/
	OutputDebugStringW(L"Hello world!!.\n");


	WNDCLASSEX wndClass = CreateMainWindowClass(hInstance, wndClassName);
	RegisterClassEx(&wndClass);

	HWND windowHandle = CreateWindowEx(
		0,
		wndClassName,
		L"Window Title?",
		WS_OVERLAPPEDWINDOW, // change here to remove window border and title bar and stuff
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Window coordinates and dimentions
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (windowHandle == NULL) {
		return -1;
	}

	ShowWindow(windowHandle, nCmdShow);

	MSG msg; // do need to zero initalize?
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); // does things with key messages
		DispatchMessage(&msg); // Calls into the WindowsProc we gave it in the WndClass
		// PostQuitMessage(0);
	}
	return 0;
}