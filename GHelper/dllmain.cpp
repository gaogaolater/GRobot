// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource.h"
#include "DB.h"
#include "MyInfo.h"
#include "Utils.h"
#include <windows.h>
INT_PTR CALLBACK DiglogFunc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    );
VOID ShowDialog(HINSTANCE hModule);
void ShowMyInfo(HWND hDlg);
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShowDialog, hModule, NULL, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

VOID ShowDialog(HINSTANCE hModule) {
	DialogBox(hModule, MAKEINTRESOURCE(IDD_MAIN), NULL, &DiglogFunc);
}

INT_PTR CALLBACK DiglogFunc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	UNREFERENCED_PARAMETER(lParam);
	list<DBNameHandle>* dbList;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		setGlobalHwnd(hDlg);
		HANDLE hookThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hookDBHandle, NULL, NULL, 0);
		if (hookThread != 0) {
			CloseHandle(hookThread);
		}
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDOK:
			ShowMyInfo(hDlg);
			break;
		case IDCANCEL:
			dbList = GetDbListHandle();
			MessageBox(hDlg, to_wstring(dbList->size()).c_str(), L"tip", MB_OK);
			break;
		default:
			break;
		}
		break;
	}
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return (INT_PTR)FALSE;
}

void ShowMyInfo(HWND hDlg) {
	MyInfo*  info = GetMyInfo();
	wchar_t text[1000];
	swprintf_s(text, L"wxid:%S,device:%S,headImage:%S,nickname:%S", 
		UnicodeToUtf8(info->wxid),
		UnicodeToUtf8(info->device), 
		UnicodeToUtf8(info->headImage),
		UnicodeToUtf8(info->nickname));
	SetDlgItemText(hDlg, IDC_LOG, text);
}

