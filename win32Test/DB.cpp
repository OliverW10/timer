#include "DB.h"
#include "State.h"
#include <stdio.h>
#include <KnownFolders.h>
#include <stdlib.h>
#include <shlobj.h> // shell objects?
#include <wininet.h>
#include <memory>
#include <atlstr.h>
#include <string>

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


// https://stackoverflow.com/a/17387176
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::wstring GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::wstring(); //No error message has been recorded
	}

	LPWSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::wstring message(messageBuffer, size);
	message.append(L", number: ");
	message.append(std::to_wstring(errorMessageID));
	message.append(L"\n");
	//ERROR_FUNCTION_NOT_CALLED

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
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
	outText.AppendFormat(L", %f", state->elapsed);
	return outText; // i think this makes a copy so doesn't matter that it goes out of scope
}

int SaveSession(HWND hWnd) {
	AppState* state = GetAppState(hWnd);

	PWSTR path;
	SHGetKnownFolderPath(FOLDERID_Documents, SHGFP_TYPE_CURRENT, NULL, &path);
	CString full_path;
	full_path.Append(path);
	full_path.Append(L"\\Timer");
	EnsureDirectoryExists(full_path);
	full_path.Append(L"\\Sessions.csv");

	OutputDebugStringW(full_path);
	OutputDebugStringW(L"\nhere!!!!\n");

	bool newlyCreated = FileExists(path);
	// despite the name CreateFile actually opens the file
	HANDLE fileHandle = CreateFileW(full_path, FILE_APPEND_DATA | FILE_GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	SetFilePointer(fileHandle, 0, NULL, FILE_END);
	CString newText = GetTextToWrite(newlyCreated, state);
	DWORD bytesWritten;
	bool successfulWrite = WriteFile(fileHandle, newText, newText.GetLength(), &bytesWritten, NULL);
	CloseHandle(fileHandle);
	return 0;
}
