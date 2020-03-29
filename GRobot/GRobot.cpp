// GRobot.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "resource.h"
#include "GRobot.h"
#include <tchar.h>
#include <string.h>
#include <string>
using namespace std;

void startWx();
INT_PTR CALLBACK wechatRobot(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
// 全局变量:
HINSTANCE hInst;                                // 当前实例

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINBOX), NULL, &wechatRobot);
	return 0;
}


INT_PTR CALLBACK wechatRobot(
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
		case ID_STARTWX:
			startWx();
			break;
		case ID_CLOSEWX:
			MessageBoxA(hDlg, "关闭按钮", "提示", NULL);
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

void startWx() {
	//注意路径一定不能写错
	wchar_t path[] = L"C:\\Program Files (x86)\\Tencent\\WeChat\\WeChat.exe";
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	BOOL bStatus = CreateProcess(path, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	if (bStatus == FALSE) {
		MessageBox(NULL, L"启动失败", L"提示", 0);
		return;
	}
}