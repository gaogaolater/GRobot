// GRobot.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "resource.h"
#include "GRobot.h"

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
			MessageBoxA(hDlg, "开始按钮", "提示", NULL);
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