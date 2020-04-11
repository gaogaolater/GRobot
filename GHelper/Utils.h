#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <string>
#include <locale.h>

DWORD GetWeChatWinAddress();
char* UnicodeToUtf8(const wchar_t* unicode);
wchar_t* UTF8ToUnicode(const char* str);
VOID setGlobalHwnd(HWND hwnd);
HWND getGlobalHwnd();
char* WcharToChar(const wchar_t* wchar);
wchar_t* CharToWchar(const char* cchar);
std::wstring StringToWstring(const std::string str);
std::string WstringToString(const std::wstring str);