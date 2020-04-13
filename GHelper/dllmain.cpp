// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource.h"
#include "DB.h"
#include "MyInfo.h"
#include "Utils.h"
#include "Message.h"
#include <windows.h>
#include <atlimage.h>
#define HOOK_LEN 5
BYTE backCOde[HOOK_LEN] = {0};

INT_PTR CALLBACK DiglogFunc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    );
VOID ShowDialog(HINSTANCE hModule);
void ShowMyInfo(HWND hDlg);
void ExecSqlClient(HWND hDlg);
void HookLoginQrcode(HWND hDlg, LPVOID funAdd);
void showPic();
void saveImg(DWORD qrcode);
void GetLoginQrcodeStr(HWND hDlg);
HWND hDlgCommon = NULL;

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
		hDlgCommon = hDlg;
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
		case IDC_HOOK:
			HookLoginQrcode(hDlg, showPic);
		case IDC_GETQRCODESTR:
			GetLoginQrcodeStr(hDlg);
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

void saveImg(DWORD qrcode) {
	// 图片长度
	DWORD picLen = qrcode + 0x4;
	// 图片的数据
	char PicData[0xFFF] = { 0 };
	size_t cpyLen = (size_t)*((LPVOID *)picLen);
	// 获取到图片数据
	memcpy(PicData, *((LPVOID *)qrcode), cpyLen);
	FILE * pFile;
	// 打开文件 不存在就创建
	fopen_s(&pFile, "qrcode.png", "wb");
	fwrite(PicData, sizeof(char), sizeof(PicData), pFile);
	fclose(pFile);
	CImage img;
	CRect rect;
	HWND PicHan =  GetDlgItem(hDlgCommon, IDC_QRCODE);
	GetClientRect(PicHan, &rect);
	// 载入图片
	img.Load(L"qrcode.png");
	img.Draw(GetDC(PicHan), rect);
}

// pushad pushfd popad popsd
DWORD pEax = 0;
DWORD pEcx = 0;
DWORD pEdx = 0;
DWORD pEbx = 0;
DWORD pEbp = 0;
DWORD pEsp = 0;
DWORD pEsi = 0;
DWORD pEdi = 0;
DWORD returnAddr = 0;
// 声明裸函数
void __declspec(naked) showPic() {
	// 备份寄存器
	__asm {
		mov pEax, eax
		mov pEcx, ecx
		mov pEdx, edx
		mov pEbx, ebx
		mov pEbp, ebp
		mov pEsp, esp
		mov pEsi, esi
		mov pEdi, edi
	}
	// 我们的二维码数据在ecx里面
	// 现在写一个函数用来保存二维码数据
	saveImg(pEcx);
	returnAddr = GetWeChatWinAddress() + 0x4DB04F;
	__asm {
		mov eax, pEax
		mov ecx, pEcx
		mov edx, pEdx
		mov ebx, pEbx
		mov ebp, pEbp
		mov esp, pEsp
		mov esi, pEsi
		mov edi, pEdi
		jmp returnAddr
	}
	
}

/*
基址 + 0x4DB04A
返回的地址4DB04F
*/
void HookLoginQrcode(HWND hDlg, LPVOID funAdd) {
	DWORD WinAddr = GetWeChatWinAddress();
	// hook地址
	DWORD hookAddr = WinAddr + 0x4DB04A;
	// 组装数据 byte 4DB04F
	BYTE jmpCode[HOOK_LEN] = { 0 };
	jmpCode[0] = 0xE9;
	*(DWORD*)&jmpCode[1] = (DWORD)(funAdd)-hookAddr - HOOK_LEN;
	// 获取自己的进程句柄
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	// 备份hook地址的数据
	if (ReadProcessMemory(hWHND, (LPCVOID)hookAddr, backCOde, HOOK_LEN, NULL) == 0) {
		MessageBoxA(NULL, "读取内存数据失败", "错误", 0);
		return;
	}
	// 写入我们组好的数据
	if (WriteProcessMemory(hWHND, (LPVOID)hookAddr, jmpCode, HOOK_LEN, NULL) == 0) {
		MessageBoxA(NULL, "内存写入失败", "错误", 0);
		return;
	}
	//MessageBoxA(NULL, "你点了hook登陆二维码按钮", "提示", 0);
}

void GetLoginQrcodeStr(HWND hDlg) {
	char qrcodeStr[500] = { 0 };
	sprintf_s(qrcodeStr, "二维码地址：http://weixin.qq.com/x/%s", (char*)*((DWORD*)(GetWeChatWinAddress() + 0x1631A88)));
	SetDlgItemTextA(getGlobalHwnd(), IDC_LOGINQRCODEINFO, qrcodeStr);
}