#include "DB.h"
#include "State.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <shlobj.h>

BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int SaveSession(HWND hWnd) {
	AppState* state = GetAppState(hWnd);

	wchar_t path_buf[256];
	SHGetKnownFolderPath(FOLDERID_ProgramData, SHGFP_TYPE_CURRENT, NULL, (wchar_t**)&path_buf);
	OutputDebugStringW(path_buf);

	//bool newlyCreated = FileExists()
	//// despite the name CreateFile actually opens the file
	//HANDLE fileHandle = CreateFileW((wchar_t*) path_buf, GENERIC_WRITE , FILE_SHARE_WRITE , NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//WriteFile(fileHandle, )
	return 0;
}