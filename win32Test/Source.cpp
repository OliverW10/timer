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

// https://stackoverflow.com/a/52514703/8847653
#define MY_PRINTF(...) {wchar_t cad[512]; swprintf(cad, 512, __VA_ARGS__);  OutputDebugString(cad);}


bool colourAlternate = 0;
void Paint(HWND windowHandle) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(windowHandle, &ps);
	HBRUSH brush = colourAlternate ? (HBRUSH)(COLOR_WINDOW + 1) : (HBRUSH)(COLOR_WINDOWTEXT + 1);
	FillRect(hdc, &ps.rcPaint, brush);
	EndPaint(windowHandle, &ps);
	colourAlternate = !colourAlternate;
	MY_PRINTF(L"%d\n", colourAlternate)
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
	default:
		return DefWindowProc(windowHandle, uMsg, wParam, lParam); // will handle the WM_QUIT?
	}
	return 0;
};

void CreateMainWindowClass(HINSTANCE applicationInstanceHandle, const wchar_t* className) {
	WNDCLASS wc = {}; // Brakets zero initialize struct
	// lpfnWndProc = long pointer to a function called Window Procedure (i love microsoft <3)
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = applicationInstanceHandle;
	wc.lpszClassName = className;
	wc.style = CS_DROPSHADOW;
	RegisterClass(&wc);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	const wchar_t WND_CLASS_NAME[] = L"IDontKnowWhatThisShouldBe";
	// Create a console to debug with. https://stackoverflow.com/questions/1333527/how-do-i-print-to-the-debug-output-window-in-a-win32-app
	/*AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);*/
	OutputDebugStringW(L"Hello world!!.\n");


	CreateMainWindowClass(hInstance, WND_CLASS_NAME);

	//HWND windowHandle = CreateWindowEx(
	//	0,
	//	WND_CLASS_NAME,
	//	L"Window Title?",
	//	WS_OVERLAPPEDWINDOW, // change here to remove window border and title bar and stuff
	//	CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Window coordinates and dimentions
	//	NULL, NULL, hInstance, NULL
	//);

	// hwnd = Handle to window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		WND_CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);
	if (hwnd == NULL) {
		return -1;
	}

	ShowWindow(hwnd, nCmdShow);

	MSG msg = {}; // do need to zero initalize?
	while (GetMessage(&msg, NULL, 0, 0)) {
		OutputDebugStringW(L"Got message\n");
		TranslateMessage(&msg); // does things with key messages
		DispatchMessage(&msg); // Calls into the WindowsProc we gave it in the WndClass
	}
	return 0;
}