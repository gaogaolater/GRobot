#include "Utils.h"
DWORD weChatWinbaseAddress;

DWORD GetWeChatWinAddress()
{
	if (weChatWinbaseAddress == 0) {
		weChatWinbaseAddress = (DWORD)LoadLibrary(L"WeChatWin.dll");
	}
	return weChatWinbaseAddress;
}

char* UnicodeToUtf8(const wchar_t* unicode)
{
    int len;
    len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
    char* szUtf8 = (char*)malloc(len + 1);
    if (szUtf8 != 0) {
        memset(szUtf8, 0, len + 1);
    }
    WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
    return szUtf8;
}

wchar_t* UTF8ToUnicode(const char* str)
{
    int    textlen = 0;
    wchar_t* result;
    textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    result = (wchar_t*)malloc((textlen + 1) * sizeof(wchar_t));
    if (result != 0)
    {
        memset(result, 0, (textlen + 1) * sizeof(wchar_t));
    }
    MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
    return    result;
}

char* WcharToChar(const wchar_t* wchar)
{
    char* m_char;
    int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
    m_char = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
    m_char[len] = '\0';
    return m_char;
}

wchar_t* CharToWchar(const char* cchar)
{
    wchar_t* m_wchar;
    int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
    m_wchar = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
    m_wchar[len] = '\0';
    return m_wchar;
}