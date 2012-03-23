// gonow_flabuilder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

BSTR getTmpFileName();
HANDLE createTmpFile(BSTR tmpFileName);
BOOL writeTmpFile(HANDLE tmpFile, BSTR flaPath);


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc <= 1) {
		wcout << "Please specify .fla path" << endl;
		return 1;
	}
	
	TCHAR path[MAX_PATH];
	TCHAR flash50Path[MAX_PATH] = TEXT("Adobe\\Adobe Flash CS5\0");
	TCHAR flash55Path[MAX_PATH] = TEXT("Adobe\\Adobe Flash CS5.5\0");


	if (SUCCEEDED(SHGetFolderPathAndSubDir(NULL, CSIDL_PROGRAM_FILESX86, NULL, SHGFP_TYPE_CURRENT, flash55Path, path))) {
		wcout << "Flash 5.5 in " << path;
	} else if (SUCCEEDED(SHGetFolderPathAndSubDir(NULL, CSIDL_PROGRAM_FILESX86, NULL, SHGFP_TYPE_CURRENT, flash50Path, path))) {
		wcout << "Flash 5.0 in " << path;
	} else {
		wcout << "No Flash??";
	}
	wcout << endl;

	if (wcslen(path) != 0) {
		BSTR tmpName = getTmpFileName();
		HANDLE tmpFile = createTmpFile(tmpName);
		if (tmpFile == INVALID_HANDLE_VALUE) {
			wcout << "Error creating tmp File" << endl;
			return 1;
		}

		if (!writeTmpFile(tmpFile, argv[1])) {
			wcout << "Error writing to File" << endl;
			return 1;
		}

		if (!CloseHandle(tmpFile)) {
			wcout << "Error closing tmp File" << endl;
			return 1;
		}

		TCHAR tmpFolder[MAX_PATH];
		GetTempPath(MAX_PATH, tmpFolder);
		string ttf = string(_com_util::ConvertBSTRToString(tmpFolder));
		ttf.append("gonow_flabuilder.jsfl");
		_bstr_t tf(ttf.c_str());
		if (!MoveFileEx(tmpName, tf, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) { 
			wcout << "Error moving tmp File" << endl;
			return 1;
		}

		string executable = string(_com_util::ConvertBSTRToString(path));
		executable.append("\\Flash.exe");
		_bstr_t ex(executable.c_str());

		string params = "\"";
		params.append(tf);
		params.append("\"");
		_bstr_t pr(params.c_str());
		
		HINSTANCE hInst = ShellExecute(0, 
			TEXT("open"), // Operation to perform
			ex, // Application name
			pr, // Additional parameters
			path, // Default directory
			SW_SHOWNOACTIVATE);
	}
	//cin.get();
	return 0;
}


BSTR getTmpFileName()
{
	TCHAR szTempFileName[MAX_PATH];  
	TCHAR lpTempPathBuffer[MAX_PATH];
	DWORD dwRetVal = 0;
	UINT uRetVal   = 0;

	dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
	if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
		wcout << "No tmp Path??" << endl;
		return NULL;
	}

	uRetVal = GetTempFileName(lpTempPathBuffer, TEXT("GONOW_FLABUILDER"), 0, szTempFileName);
	if (uRetVal == 0) {
		wcout << "No tmp File??" << endl;
		return NULL;
	}
	wcout << "Tmp file: " << szTempFileName << endl;
	BSTR tmp = SysAllocString(szTempFileName);
	return tmp;
}


HANDLE createTmpFile(BSTR tmpFileName)
{
	HANDLE hTempFile = INVALID_HANDLE_VALUE;

	hTempFile = CreateFile((LPTSTR) tmpFileName, // file name 
                           GENERIC_WRITE,        // open for write 
                           0,                    // do not share 
                           NULL,                 // default security 
                           CREATE_ALWAYS,        // overwrite existing
                           FILE_ATTRIBUTE_NORMAL,// normal file 
                           NULL);                // no template 
	if (hTempFile == INVALID_HANDLE_VALUE) { 
		wcout << "Error creating tmp File" << endl;
	}

	return hTempFile;
}

BOOL writeTmpFile(HANDLE tmpFile, BSTR flaPath)
{
	BOOL fSuccess = FALSE;
	string p = string(_com_util::ConvertBSTRToString(flaPath));
	replace(p.begin(), p.end(), '\\', '/');

	string buf = "var reqdoc = \"";
	buf.append(p);
	buf.append("\";\n\
var reqdocarr = reqdoc.split(\"/\");\n\
var reqname = reqdocarr[reqdocarr.length - 1];\n\
var seldoc = null;\n\
var docs = fl.documents;\n\
\n\
for (doc in docs) {\n\
	if (docs[doc].name == reqname.substr(0, reqname.length - 4)) {\n\
		seldoc = docs[doc];\n\
	}\n\
}\n\
if (!seldoc) {\n\
	seldoc = fl.openDocument(\"file:///\" + reqdoc);\n\
}\n\
\n\
if (seldoc) {\n\
	seldoc.publish();\n\
} else {\n\
	alert(\"Document \" + seldoc + \"not found\");\n\
}\n");
	
	DWORD dwBytesWritten = 0;

	fSuccess = WriteFile(tmpFile, buf.c_str(), buf.size(), &dwBytesWritten, NULL);
	return fSuccess;
}
