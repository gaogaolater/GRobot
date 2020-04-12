#include "Message.h"
#include "Utils.h"
#include "pch.h"
#include <tchar.h>
#include <Shlwapi.h>
#include <string>
#include "resource.h"
#include "EasyLog.h"
//必须引入以下库 否则StrStrA有问题
#pragma comment(lib,"Shlwapi.lib")
using namespace std;

//返回的地址
DWORD msgHookAddress;
//参数地址
DWORD paramAddress;
//esp地址
DWORD r_esp;

//读取内存中的字符串
//存储格式
//xxxxxxxx:字符串地址（memAddress）
//xxxxxxxx:字符串长度（memAddress +4）
LPCWSTR GetMsgByAddress(DWORD memAddress)
{
	//获取字符串长度
	DWORD msgLength = *(DWORD*)(memAddress + 4);
	if (msgLength == 0)
	{
		WCHAR* msg = new WCHAR[1];
		msg[0] = 0;
		return msg;
	}

	WCHAR* msg = new WCHAR[msgLength + 1];
	ZeroMemory(msg, msgLength + 1);

	//复制内容
	wmemcpy_s(msg, msgLength + 1, (WCHAR*)(*(DWORD*)memAddress), msgLength + 1);
	return msg;
}

struct Msg {
	wchar_t* text;
	int len;//长度
	int type;//类型
};
wstring msgAll;
void ReceiveMsg() {
	EasyLog::Write("address before===" + to_string(r_esp));
	DWORD** espPtr = (DWORD**)r_esp;
	//eax+0x40 发送人id
	//eax+0x44 长度  10个人  14群消息
	//eax+0x48 类型
	msgAll.append(GetMsgByAddress(**espPtr + 0x40));
	msgAll.append(TEXT(":"));
	msgAll.append(GetMsgByAddress(**espPtr + 0x68));
	msgAll.append(TEXT("\r\n"));
	EasyLog::Write(WstringToString(msgAll));
	SetDlgItemText(getGlobalHwnd(), IDC_MESSAGE, msgAll.c_str());
	//EasyLog::Write((char*)(**espPtr + 0x40));
	//sender.len = (int)(**espPtr + 0x44);
	//EasyLog::Write("32");
	//sender.type = (int)(**espPtr + 0x48);
	//EasyLog::Write("34");

	////eax+0x68 消息
	////eax+0x6C 长度
	////eax+0x70 类型 4文本消息  800文件   400图片  200语音
	//Msg msg;
	//swprintf_s(msg.text, L"%S", (char*)(**espPtr + 0x68));
	//EasyLog::Write("41");
	//msg.len = (int)(**espPtr + 0x6C);
	//msg.type = (int)(**espPtr + 0x70);
	//wchar_t senderStr[1000];
	//swprintf_s(senderStr, L"发送者:%s 长度:%d 类型:%d", sender.text, sender.len, sender.type);
	//EasyLog::Write("46");
	//msgAll += senderStr;
	//msgAll += L"\r\n";
	//wchar_t sendContent[2000];
	//swprintf_s(sendContent, L"内容:%s 长度:%d 类型:%d", msg.text, msg.len, msg.type);
	//msgAll += sendContent;
	//msgAll += L"\r\n";
	//EasyLog::Write("53");
	//SetDlgItemText(getGlobalHwnd(), IDC_MESSAGE, msgAll.c_str());
}

__declspec(naked) void My_Hook_Message()
{
	__asm
	{
		//补充hook占用的代码
		mov ecx, paramAddress
		mov r_esp, esp
		pushad
		call ReceiveMsg
		popad
		jmp msgHookAddress
	}
}

void HookMessage() {
	DWORD dllAddr;
	while (true) {
		dllAddr = GetWeChatWinAddress();
		if (dllAddr == 0) {
			Sleep(100);
		}
		else {
			break;
		}
	}
	EasyLog::Write("HookMessageStart");
	/*
	784C28BD    50              push eax
	784C28BE    A1 40FA7979     mov eax,dword ptr ds:[0x7979FA40]   
	784C28C3    B9 40FA7979     mov ecx,WeChatWi.7979FA40	//hook offset 162fa40
	784C28C8    FF50 08         call dword ptr ds:[eax+0x8]
	784C28CB    8B1D F8197A79   mov ebx,dword ptr ds:[0x797A19F8]
	784C28D1    F6C3 01         test bl,0x1 
	*/

	DWORD hookAddress = dllAddr + 0x3528C3;//0x3528C3
	msgHookAddress = hookAddress + 5;
	paramAddress = dllAddr + 0x162fa40;
	BYTE jumpCode[5] = { 0 };
	jumpCode[0] = 0xE9;//jmp
	*(DWORD*)&jumpCode[1] = (DWORD)My_Hook_Message - hookAddress - 5;
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookAddress, jumpCode, 5, NULL);
	EasyLog::Write("HookMessageEnd");
}