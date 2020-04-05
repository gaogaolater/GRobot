// GRobot.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "resource.h"
#include "GRobot.h"
#include <tchar.h>
#include <string.h>
#include <string>
#include <direct.h>
#include "OpenWeChat.h"
#include "Utils.h"

using namespace std;

void StartWeChat(HWND hDlg);
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
	char dest[100];
	const char src[100] = "哈哈哈";
	//swprintf_s(dest, L"1111%S-haha", UnicodeToUtf8(src));
	sprintf_s(dest, "%s", src);
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
			StartWeChat(hDlg);
			break;
		case ID_CLOSEWX:
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

void StartWeChat(HWND hDlg) {
	//注意路径一定不能写错
	wchar_t path[] = L"C:\\Program Files (x86)\\Tencent\\WeChat\\WeChat.exe";
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	BOOL bStatus = CreateProcess(path, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	if (bStatus == FALSE) {
		MessageBox(NULL, L"启动失败", L"提示", 0);
		return;
	}
	char dllPath[MAX_PATH] = { 0 };
	//这里发布的时候Debug要去除
	sprintf_s(dllPath, "%s\\Debug\\%s", _getcwd(NULL, 0), "GHelper.dll");
	//为dll的路径字符串开辟内存
	LPVOID pathPointer = VirtualAllocEx(pi.hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pathPointer == 0)
	{
		MessageBoxA(NULL, "VirtualAllocEx失败", "错误", 0);
		return;
	}
	//把写入微信进程
	WriteProcessMemory(pi.hProcess, pathPointer, dllPath, MAX_PATH, NULL);
	//获取函数地址
	FARPROC address = GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryA");
	//调用LoadLibraryA函数
	HANDLE hRemote = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, pathPointer, 0, NULL);
	ResumeThread(pi.hThread);
	//CloseHandle(pi.hProcess);
	//CloseHandle(pi.hThread);
}