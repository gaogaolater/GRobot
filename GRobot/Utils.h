#pragma once
#include <Windows.h>
DWORD GetWeChatWinAddress();
char* UnicodeToUtf8(const wchar_t* unicode);
wchar_t* UTF8ToUnicode(const char* str);
char* WcharToChar(const wchar_t* wchar);
wchar_t* CharToWchar(const char* cchar);