#include "GUI.h"
#include "State.h"
#include <stdio.h>


// returns old font
HFONT CreateAndSetFont(HDC hdc, int size, HFONT* newFont_out) {
	HFONT font = CreateFont(size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Candara"));
	*newFont_out = font; // TODO: is font on stack?
	return (HFONT)SelectObject(hdc, font);
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
	AppState *appState = GetAppState(windowHandle);
	appState->elapsed = GetTimeSinceFirstCall();

	PAINTSTRUCT ps;
	// handle to device context
	HDC hdc = BeginPaint(windowHandle, &ps);

	FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

	HFONT fontBig;
	HFONT oldFont = CreateAndSetFont(hdc, 64, &fontBig);
	SetTextColor(hdc, RGB(255, 0, 0));

	wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t) * 128);
	// Convert into to wide string
	swprintf_s(str, 128, L"%.1f", appState->elapsed);
	RECT timerTextRect = ps.rcPaint;
	timerTextRect.top -= 7;
	timerTextRect.bottom -= 7;
	DrawText(hdc, str, -1, &timerTextRect, DT_CENTER | DT_VCENTER);

	HFONT fontSmall;
	CreateAndSetFont(hdc, 12, &fontSmall);
	SetTextColor(hdc, RGB(0, 0, 0));
	RECT programTitleRect = ps.rcPaint;
	programTitleRect.top = 50;
	programTitleRect.bottom = 75;
	programTitleRect.left -= 5;
	programTitleRect.right += 5;
	DrawText(hdc, appState->appName, -1, &programTitleRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);

	free(str);

	// restore old font
	SelectObject(hdc, oldFont);
	DeleteObject(fontBig);
	DeleteObject(fontSmall);
	EndPaint(windowHandle, &ps);
}




HCURSOR hCursor = NULL;
bool SetCursor(LPARAM lParam) {
	if (hCursor == NULL) {
		// long pointer to constant TCHAR (wchar or char) string
		LPCTSTR cursor = IDC_SIZEALL; // not working for some reason
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


HWND CreateTimerWindow(WNDPROC windowProc, HINSTANCE hInstance, int nCmdShow, AppState* appState) {
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
		L"Timer App",    // Window text
		WS_EX_TOPMOST | WS_POPUP,            // Window style   WS_EX_TRANSPARENT

		// Size and position
		100, 100, 120, 75,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		appState        // Additional application data
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