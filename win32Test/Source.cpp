// TODO
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


HFONT hFont;
void SetMyFont(HDC hdc) {
	if (hFont == NULL) {
		//hFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
		hFont = CreateFont(64, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Candara"));
	}
	(HFONT)SelectObject(hdc, hFont);
}

ULARGE_INTEGER startTime;
bool hasSetStartTime = false;
double GetTimeSinceFirstCall() {
	ULARGE_INTEGER currentTime;
	GetSystemTimeAsFileTime((LPFILETIME) & currentTime);
	if (!hasSetStartTime) {
		startTime = currentTime;
		hasSetStartTime = true;
	}
	startTime.QuadPart += 1;
	// in 100ns intervals
	double delta = (currentTime.QuadPart - startTime.QuadPart) / 10'000'000.0;
	return delta;
}

bool colourAlternate = 0;

void Paint(HWND windowHandle) {
	
	PAINTSTRUCT ps;
	// handle to device context
	HDC hdc = BeginPaint(windowHandle, &ps);
	
	SetMyFont(hdc);
	SetTextColor(hdc, RGB(255, 0, 0));
	SetBkMode(hdc, TRANSPARENT); // didnt work

	FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

	float timerSeconds = GetTimeSinceFirstCall();

	wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t)*128); // TODO memory leak
	// Convert into to wide string
	swprintf_s(str, 128, L"%.1f", timerSeconds);

	//OutputDebugStringW(str);
	DrawText(hdc, str, -1, &ps.rcPaint, DT_CENTER | DT_VCENTER);

	EndPaint(windowHandle, &ps);
	colourAlternate = !colourAlternate;
}

HCURSOR hCursor = NULL;
bool SetCursor(LPARAM lParam) {
	if (hCursor == NULL) {
		LPCTSTR cursor = IDC_ARROW;
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
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		// need to cast because its compiled with c++
		wchar_t* msg = (wchar_t*)malloc(32 * sizeof(wchar_t));
		if(msg == NULL) return 1; // shut up warning

		StringCbPrintfW(msg, 32, L"Resized: %d, %d\n", width, height);
		// Puts it in 'Output' console in vs
		OutputDebugStringW(msg);
	}
		break;
	case WM_PAINT:
		Paint(windowHandle);
		break;
	case WM_CLOSE: // When close button or alt-f4 is pressed
		DestroyWindow(windowHandle);
		break;
	case WM_DESTROY: // After main window is destoryed
		PostQuitMessage(0); // queues a WM_QUIT
		break;
	case WM_SETCURSOR:
		return SetCursor(lParam);
	case WM_ERASEBKGND:
		return 0;
	default:
		return DefWindowProc(windowHandle, uMsg, wParam, lParam); // will handle the WM_QUIT?
	}
	return 0;
};

DWORD WINAPI TimerThread(LPVOID hWnd) {

	while (true) {
		Sleep(20);
		RedrawWindow((HWND)hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	const wchar_t WND_CLASS_NAME[] = L"IDontKnowWhatThisShouldBe";

	WNDCLASS windowClass = {}; // Brakets zero initialize struct
	// lpfnWndProc = long pointer to a function called Window Procedure (i love microsoft <3)
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = WND_CLASS_NAME;
	//windowClass.hbrBackground = 
	RegisterClass(&windowClass);

	// hwnd = Handle to window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		WND_CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		(WS_EX_TOPMOST | WS_POPUP) & ~WS_CLIPCHILDREN,            // Window style WS_EX_TRANSPARENT

		// Size and position
		100, 100, 120, 75,

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