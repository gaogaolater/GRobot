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
//esp地址
DWORD r_esp;

struct Msg {
	wchar_t text[2000];
	int len;//长度
	int type;//类型
};
wstring msgAll;
void ReceiveMsg() {
	EasyLog::Write("address before===" + to_string(r_esp));
	DWORD** espPtr = (DWORD**)r_esp;
	EasyLog::Write("address after===" + to_string(**espPtr));
	//eax+0x40 发送人id
	//eax+0x44 长度  10个人  14群消息
	//eax+0x48 类型
	Msg sender;

	swprintf_s(sender.text, L"%s", (wchar_t*)*(DWORD*)(**espPtr + 0x40));
	EasyLog::Write(WcharToChar(sender.text));
	wchar_t content[1000];
	swprintf_s(content, L"%s", (wchar_t*)*(DWORD*)(**espPtr + 0x68));
	EasyLog::Write(WcharToChar(content));
	msgAll += sender.text;
	msgAll += L" : ";
	msgAll += content;
	msgAll += L"\r\n";
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
		mov eax, dword ptr ds : [0x7979FA40]
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
	(hook)784C28BE    A1 40FA7979     mov eax,dword ptr ds:[0x7979FA40]
	784C28C3    B9 40FA7979     mov ecx,WeChatWi.7979FA40
	784C28C8    FF50 08         call dword ptr ds:[eax+0x8]
	784C28CB    8B1D F8197A79   mov ebx,dword ptr ds:[0x797A19F8]
	784C28D1    F6C3 01         test bl,0x1
	*/
	DWORD hookAddress = dllAddr + 0x3528BE;
	msgHookAddress = hookAddress + 5;
	BYTE jumpCode[5] = { 0 };
	jumpCode[0] = 0xE9;//jmp
	*(DWORD*)&jumpCode[1] = (DWORD)My_Hook_Message - hookAddress - 5;
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookAddress, jumpCode, 5, NULL);
	EasyLog::Write("HookMessageEnd");
}