// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource.h"
#include "DB.h"
#include "MyInfo.h"
#include "Utils.h"
#include "Message.h"
#include <windows.h>
INT_PTR CALLBACK DiglogFunc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    );
VOID ShowDialog(HINSTANCE hModule);
void ShowMyInfo(HWND hDlg);
void ExecSqlClient(HWND hDlg);
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
	switch (message)
	{
	case WM_INITDIALOG:
	{
		setGlobalHwnd(hDlg);
		HANDLE hookDBThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HookDBHandle, NULL, NULL, 0);
		if (hookDBThread != 0) {
			CloseHandle(hookDBThread);
		}
		HANDLE hookMsgThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HookMessage, NULL, NULL, 0);
		if (hookMsgThread != 0) {
			CloseHandle(hookMsgThread);
		}
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDBTNMYINFO:
			ShowMyInfo(hDlg);
			break;
		case IDBTNSQL:
			ExecSqlClient(hDlg);
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

//sql查询结果
wstring sqlResult;
int SQLCallback(void* para, int nColumn, char** colValue, char** colName) 
{
	for (int i = 0; i < nColumn; i++)
	{
		char row[1000];
		sprintf_s(row, " %s:%s ",*(colName + i), colValue[i]);
		sqlResult += CharToWchar(row);
	}
	sqlResult += L"\r\n";
	SetDlgItemText(getGlobalHwnd(), IDC_SQLRESULT, sqlResult.c_str());
	return 0;
}

void ExecSqlClient(HWND hDlg) {
	sqlResult = L"";
	char* sqlErrmsg = NULL;
	char dbName[0x100] = { 0 };
	GetDlgItemTextA(hDlg, IDC_COMBODB, dbName, 100);
	char sql[0x1000] = { 0 };
	GetDlgItemTextA(hDlg, IDC_EBSQL, sql, 1000);
	ExecSql(dbName,sql, SQLCallback, sqlErrmsg);
}

void ShowMyInfo(HWND hDlg) {
	MyInfo*  info = GetMyInfo();
	wchar_t text[1000];
	swprintf_s(text, L"wxid:%S,device:%S,headImage:%S,nickname:%S", 
		UnicodeToUtf8(info->wxid),
		UnicodeToUtf8(info->device), 
		UnicodeToUtf8(info->headImage),
		UnicodeToUtf8(info->nickname));
	SetDlgItemText(hDlg, IDC_EBLOG, text);
}

