#include <Windows.h>
#include "Error.h"

void ErrorBox(const std::string &msg)
{
	// multibyte string to wide char (UTF-16) string // TODO: global function
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, msg.c_str(), -1, NULL, 0);
	wchar_t wstrName[100];
	MultiByteToWideChar(CP_UTF8, 0, msg.c_str(), -1, wstrName, wchars_num);

	MessageBox(0, wstrName, L"ERROR", MB_OK);
	exit(0);
}