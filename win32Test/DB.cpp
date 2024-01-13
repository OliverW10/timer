#include "DB.h"
#include "State.h"
#include <stdio.h>
#include <KnownFolders.h>
#include <stdlib.h>
#include <shlobj.h> // shell objects?
#include <wininet.h>
#include <memory>
#include <atlstr.h>

#pragma comment(lib, "Wininet")

BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// thx chatgpt
bool EnsureDirectoryExists(LPCWSTR directoryPath) {
	if (CreateDirectory(directoryPath, nullptr) ||
		GetLastError() == ERROR_ALREADY_EXISTS) {
		//std::wcout << L"Directory '" << directoryPath << L"' created or already exists." << std::endl;
		return true;
	}
	else {
		//std::cerr << L"Failed to create directory '" << directoryPath << L"'. Error code: " << GetLastError() << std::endl;
		return false;
	}
}


PCWSTR csvHeader = L"ProgName, EndDateTime, DurationSeconds\n";

LPCTSTR GetTextToWrite(bool isFirst, AppState* state) {
	CString outText;
	if (isFirst) {
		outText.Append(csvHeader);
	}
	outText.Append(state->appName);
	outText.Append(L", ");
	TCHAR *dateBuf = new TCHAR[INTERNET_RFC1123_BUFSIZE];
	SYSTEMTIME currentTime;
	GetSystemTime(&currentTime);
	InternetTimeFromSystemTime(&currentTime, INTERNET_RFC1123_FORMAT, dateBuf, INTERNET_RFC1123_BUFSIZE);
	outText.Append(dateBuf);
	outText.AppendFormat(L", %d", state->elapsed);
	return outText; // i think this makes a copy so doesn't matter that it goes out of scope
}

int SaveSession(HWND hWnd) {
	AppState* state = GetAppState(hWnd);

	PWSTR path;
	SHGetKnownFolderPath(FOLDERID_ProgramData, SHGFP_TYPE_CURRENT, NULL, &path);
	CString full_path;
	full_path.Append(path);
	full_path.Append(L"\\Timer");
	EnsureDirectoryExists(full_path);
	full_path.Append(L"\\Sessions.csv");

	OutputDebugStringW(full_path);
	OutputDebugStringW(L"\nhere!!!!\n");

	bool newlyCreated = FileExists(path);
	// despite the name CreateFile actually opens the file
	HANDLE fileHandle = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_WRITE , NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		//throw - 1;
		OutputDebugStringW(L"failed to create file\n");
		return -1;
	}
	SetFilePointer(fileHandle, 0, NULL, FILE_END);
	CString newText = GetTextToWrite(newlyCreated, state);
	WriteFile(fileHandle, newText, newText.GetLength(), NULL, NULL);
	CloseHandle(fileHandle);
	return 0;
}