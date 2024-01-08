// TODO
// - No window, just text
// - Show in system tray (called notifications)
// - Shortcuts (need to register or can just get events?)
// - SQLite
// - Stop on sleep?
// - Autostart / Installer (msix?)


#include <Windows.h>
#include <stdio.h>
#include <strsafe.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

// https://stackoverflow.com/a/52514703/8847653
#define MY_PRINTF(...) {wchar_t cad[512]; swprintf(cad, 512, __VA_ARGS__);  OutputDebugString(cad);}

HFONT hFont = NULL;
bool colourAlternate = 0;
uint64_t startTime;

void Paint(HWND windowHandle) {
	if (hFont == NULL) {
		hFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	}
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(windowHandle, &ps);
	//HBRUSH brush = colourAlternate ? (HBRUSH)(COLOR_WINDOW + 1) : (HBRUSH)(COLOR_WINDOWTEXT + 1);
	//FillRect(hdc, &ps.rcPaint, brush);
	HFONT hFont;
	hFont = CreateFont(48, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Candara"));
	HFONT hFontOriginal = (HFONT)SelectObject(hdc, hFont);

	SetTextColor(hdc, RGB(255, 0, 0));

	SYSTEMTIME currentTimeParts;
	GetSystemTime(&currentTimeParts);
	uint64_t currentTime;
	SystemTimeToFileTime(&currentTimeParts, (LPFILETIME)&currentTime);
	int delta = startTime - currentTime;

	wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t)*128); // TODO memory leak
	swprintf_s(str, 128, L"%d", delta);
	//OutputDebugStringW(str);
	DrawText(hdc, str, -1, &ps.rcPaint, DT_CENTER | DT_VCENTER);

	EndPaint(windowHandle, &ps);
	colourAlternate = !colourAlternate;
}

HCURSOR hCursor = NULL;
bool SetCursor(LPARAM lParam) {
	if (hCursor == NULL) {
		LPCTSTR cursor = IDC_NO;
		hCursor = LoadCursor(NULL, cursor);
	}
	if (LOWORD(lParam) == HTCLIENT)
	{
		SetCursor(hCursor);
		return true;
	}
}

const int wordMask = (1 << 16) - 1;
LRESULT CALLBACK WindowProc(HWND windowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_SIZE:
	{
		int width = lParam & wordMask;
		int height = (lParam >> 16) & wordMask;
		// need to cast because its c++
		wchar_t* msg = (wchar_t*)malloc(32 * sizeof(wchar_t));
		if(msg == NULL) return 1; // shut up warning

		StringCbPrintfW(msg, 32, L"Resized: %d, %d\n", width, height);
		// Puts it in 'Output' console in vs
		OutputDebugStringW(msg);
	}
		break;
	case WM_PAINT:
	{
		Paint(windowHandle);
	}
		break;
	case WM_CLOSE: // When close button or shortcut is pressed
		DestroyWindow(windowHandle);
		break;
	case WM_DESTROY: // After main window is destoryed
		PostQuitMessage(0); // queues a WM_QUIT
		break;
	case WM_SETCURSOR:
		return SetCursor(lParam);
	default:
		return DefWindowProc(windowHandle, uMsg, wParam, lParam); // will handle the WM_QUIT?
	}
	return 0;
};

DWORD WINAPI TimerThread(LPVOID hWnd) {

	while (true) {
		Sleep(100);
		UpdateWindow((HWND)hWnd);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	const wchar_t WND_CLASS_NAME[] = L"IDontKnowWhatThisShouldBe";

	WNDCLASS wc = {}; // Brakets zero initialize struct
	// lpfnWndProc = long pointer to a function called Window Procedure (i love microsoft <3)
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WND_CLASS_NAME;
	RegisterClass(&wc);

	// hwnd = Handle to window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		WND_CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_EX_TOPMOST | WS_POPUP,            // Window style WS_EX_TRANSPARENT

		// Size and position
		100, 100, 200, 100,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);
	if (hwnd == NULL) {
		return -1;
	}

	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SYSTEMTIME startTimeParts;
	GetSystemTime(&startTimeParts);
	SystemTimeToFileTime(&startTimeParts, (LPFILETIME)&startTime);

	DWORD threadId;
	HANDLE hThread = CreateThread(
		NULL,
		0,
		TimerThread,
		hwnd,
		0,
		&threadId
	);
	if (hThread == NULL) {
		OutputDebugStringW(L"failed to create thread\n");
		return -1;
	}



	OutputDebugStringW(L"Capturing\n");
	HWND foregroundWindow = GetForegroundWindow();
	wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t) * 128);
	GetWindowTextW(foregroundWindow, str, 128);
	OutputDebugStringW(str);
	// if the titles are changing can swap to using GetWindowThreadProcessId then GetModuleFileName to get the exe path/name

	ShowWindow(hwnd, nCmdShow);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); // does things with key messages
		DispatchMessage(&msg); // Calls into the WindowsProc we gave it in the WndClass
	}
	return 0;
}