#pragma once
#include <Windows.h>
DWORD GetWeChatWinAddress();
char* UnicodeToUtf8(const wchar_t* unicode);
wchar_t* UTF8ToUnicode(const char* str);