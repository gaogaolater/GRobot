// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource.h"

INT_PTR CALLBACK DiglogFunc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);
VOID ShowDialog(HINSTANCE hModule);

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShowDialog, hModule, NULL, 0);
		break;
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
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDOK:
			MessageBox(NULL, L"好了", L"tip", MB_OK);
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