#include "GUI.h"
#include <stdio.h>


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
	GetSystemTimeAsFileTime((LPFILETIME)&currentTime);
	if (!hasSetStartTime) {
		startTime = currentTime;
		hasSetStartTime = true;
	}
	startTime.QuadPart += 1;
	// in 100ns intervals
	double delta = (currentTime.QuadPart - startTime.QuadPart) / 10'000'000.0;
	return delta;
}

void Paint(HWND windowHandle) {

	PAINTSTRUCT ps;
	// handle to device context
	HDC hdc = BeginPaint(windowHandle, &ps);

	SetMyFont(hdc);
	SetTextColor(hdc, RGB(255, 0, 0));
	SetBkMode(hdc, TRANSPARENT); // didnt work

	FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

	float timerSeconds = GetTimeSinceFirstCall();

	wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t) * 128); // TODO memory leak
	// Convert into to wide string
	swprintf_s(str, 128, L"%.1f", timerSeconds);

	//OutputDebugStringW(str);
	DrawText(hdc, str, -1, &ps.rcPaint, DT_CENTER | DT_VCENTER);

	EndPaint(windowHandle, &ps);
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

DWORD WINAPI TimerThread(LPVOID hWnd) {

	while (true) {
		Sleep(20);
		RedrawWindow((HWND)hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
	}
}


HWND CreateTimerWindow(WNDPROC windowProc, HINSTANCE hInstance, int nCmdShow) {
	//// Create Window Class
	const wchar_t WND_CLASS_NAME[] = L"IDontKnowWhatThisShouldBe";
	WNDCLASS windowClass = {}; // Brakets zero initialize struct
	// lpfnWndProc = long pointer to a function called Window Procedure (i love microsoft <3)
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = WND_CLASS_NAME;
	//windowClass.hbrBackground = 
	RegisterClass(&windowClass);

	//// Create Instance of Window
	HWND hwnd = CreateWindowEx(
		// hwnd = Handle to window
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


	//// Create Thread that triggers redraws at intervals
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
	}


	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(hwnd, nCmdShow);

	return hwnd;
}