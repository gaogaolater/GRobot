﻿#include "Message.h"
#include "Utils.h"
#include "pch.h"
#include <tchar.h>
#include <Shlwapi.h>
#include <string>
#include "resource.h"
#include "EasyLog.h"
#include "offset.h"
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

/*
基址：6a9f0000

6AAD234D    E8 9EC1F6FF     call WeChatWi.6AA3E4F0
6AAD2352    8B55 B0         mov edx,dword ptr ss:[ebp-0x50]
6AAD2355    8D43 14         lea eax,dword ptr ds:[ebx+0x14]
6AAD2358    6A 01           push 0x1
6AAD235A    50              push eax
6AAD235B    53              push ebx
6AAD235C    8D8D 84F7FFFF   lea ecx,dword ptr ss:[ebp-0x87C]
6AAD2362    E8 B9562400     call WeChatWi.6AD17A20
6AAD2367    83C4 0C         add esp,0xC
6AAD236A    50              push eax

edx 数据：
0DF3D550  111D4E48  UNICODE "wb362115359"
0DF3D554  0000000B
0DF3D558  00000020

eax = 0

ebx
0E09005C  1184C7C8  UNICODE "已经好了啊"
0E090060  00000005
0E090064  00000008
0E090068  00000000
0E09006C  00000000
0E090070  00000000
0E090074  00000000
0E090078  00000000
0E09007C  00000000
*/

struct SendMsgStruct {
	wchar_t* text;
	int length;
	int bufLen;
};

void SendText(wchar_t* wxid, wchar_t* msg)
{
	wstring log = L"";
	log += wxid;
	log += L":";
	log += msg;
	EasyLog::Write(WstringToString(log));
	EasyLog::Write("HookMessageStart");
	BYTE buff[0x87c] = { 0 };
	SendMsgStruct struct_wxid;
	struct_wxid.text = wxid;
	struct_wxid.length = wcslen(wxid);
	struct_wxid.bufLen = wcslen(wxid) * 2;
	SendMsgStruct struct_msg;
	struct_msg.text = msg;
	struct_msg.length = wcslen(msg);
	struct_msg.bufLen = wcslen(msg) * 2;

	DWORD* wxidPtr = (DWORD*)&struct_wxid.text;
	DWORD* msgPtr = (DWORD*)&struct_msg.text;

	DWORD sendTextAddr = GetWeChatWinAddress() + 0x327A20;
	__asm {
		pushad
		mov edx, wxidPtr
		mov ebx, msgPtr
		push 0x1
		push 0
		push ebx
		lea ecx, buff
		call sendTextAddr
		add esp, 0xC
		popad
	}
}

/*
基址 78380000

7846245A  |> \53            |push ebx                                ;  图片临时路径; Case 2 of switch 784622B2
7846245B  |.  FF75 B0       |push [local.20]                         ;  接收人wxid
7846245E  |.  8D85 44FBFFFF |lea eax,[local.303]                     ;  buff  lea eax,dword ptr ss:[ebp-0x4BC]
78462464  |.  50            |push eax
78462465  |.  51            |push ecx                                ;  0
78462466  |.  E8 85C0F6FF   |call WeChatWi.783CE4F0
7846246B  |.  83C4 04       |add esp,0x4
7846246E  |.  8BC8          |mov ecx,eax
78462470      E8 9B4F2400   call WeChatWi.786A7410

ebx
16CEC49C  16625C00  UNICODE "C:\Users\gaogao\AppData\Local\Temp\1586912818.jpg"
16CEC4A0  00000031
16CEC4A4  00000040

[local.20]
11ED85E0  119964A0  UNICODE "wb362115359"
11ED85E4  0000000B
11ED85E8  00000010

eax 
buff  lea eax,dword ptr ss:[ebp-0x4BC]

ecx 0
lea ecx,dword ptr ss:[ebp-0x78]

发送图片会有图片占用情况，建议先复制到固定的目录再发送
*/
void SendImage(wchar_t* wxid, wchar_t* imagePath)
{
	DWORD call1 = GetWeChatWinAddress() + 0x4E4F0;
	DWORD call2 = GetWeChatWinAddress() + 0x327410;
	SendMsgStruct wxidStuct;
	wxidStuct.text = wxid;
	wxidStuct.length = wcslen(wxid);
	wxidStuct.bufLen = wcslen(wxid) * 2;
	SendMsgStruct imagePathStuct;
	imagePathStuct.text = imagePath;
	imagePathStuct.length = wcslen(imagePath);
	imagePathStuct.bufLen = wcslen(imagePath) * 2;
	BYTE buff[0x4BC] = { 0 };
	DWORD r_esp = 0;
	__asm 
	{
		mov r_esp, esp
		lea ebx, imagePathStuct
		push ebx
		lea eax, wxidStuct
		push eax
		lea eax, buff
		push eax
		push 0
		call call1
		add esp, 0x4
		mov ecx, eax
		call call2
		mov eax, r_esp
		mov esp, eax
	}
}
/*
基址 78380000

784625D9  |.  FF75 A4       |push [local.23]                         ;  start 0
784625DC  |.  83EC 14       |sub esp,0x14
784625DF  |.  8BCC          |mov ecx,esp
784625E4  |.  6A FF         |push -0x1
784625E6  |.  68 A8586E79   |push WeChatWi.796E58A8
784625EB  |.  E8 00A33F00   |call WeChatWi.7885C8F0
784625F0  |.  83EC 14       |sub esp,0x14
784625F3  |.  8BCC          |mov ecx,esp
784625F8  |.  53            |push ebx                                ;  文件路径
784625F9  |.  E8 32A33F00   |call WeChatWi.7885C930
784625FE  |.  83EC 14       |sub esp,0x14
78462601  |.  8BCC          |mov ecx,esp
78462606  |.  FF75 B0       |push [local.20]                         ;  发送对象的wxid
78462609  |.  E8 22A33F00   |call WeChatWi.7885C930
7846260E  |.  8D85 44FBFFFF |lea eax,[local.303]                     ;  buffer
78462618  |.  50            |push eax
78462619  |.  E8 D21DF7FF   |call WeChatWi.783D43F0                  ;  组合发送数据
7846261E      8BC8          |mov ecx,eax
78462620      C645 FC 0C    |mov byte ptr ss:[ebp-0x4],0xC
78462624      E8 87B11700   |call WeChatWi.785DD7B0                  ;  发送文件

文件结构
0F4793DC  0696CF48  UNICODE "\\Mac\Home\Desktop\Windows\10.txt"
0F4793E0  00000021
0F4793E4  00000040
0F4793E8  00000000
0F4793EC  00000000
0F4793F0  00000000
0F4793F4  00000000
0F4793F8  00000000
0F4793FC  00000000

wxid结构
0F3921B0  04B00220  UNICODE "filehelper"
0F3921B4  0000000A
0F3921B8  00000010
0F3921BC  00000000
0F3921C0  00000000
0F3921C4  00000000

*/
void SendFile(wchar_t* wxid, wchar_t* filePath)
{
	struct FileStruct {
		wchar_t* text;
		int len = 0;
		int maxLen = 0;
		int buff[0x18] = { 0 };
	};

	struct WxidStruct {
		wchar_t* text;
		int len = 0;
		int maxLen = 0;
		int buff[0xC] = { 0 };
	};

	FileStruct fileStruct;
	fileStruct.text = filePath;
	fileStruct.len = wcslen(filePath);
	fileStruct.maxLen = wcslen(filePath) * 2;
	WxidStruct wxidStruct;
	wxidStruct.text = wxid;
	wxidStruct.len = wcslen(wxid);
	wxidStruct.maxLen = wcslen(wxid) * 2;
	DWORD param1 = GetWeChatWinAddress() + MSG_FILE_PARAM1;
	DWORD call1 = GetWeChatWinAddress() + MSG_FILE_CALL1;
	DWORD call2 = GetWeChatWinAddress() + MSG_FILE_CALL2;
	DWORD call3 = GetWeChatWinAddress() + MSG_FILE_CALL3;
	DWORD call4 = GetWeChatWinAddress() + MSG_FILE_CALL4;
	char* wxidPtr = (char*)&wxidStruct;
	char* pathPtr = (char*)&fileStruct;
	char buff[0x4BC] = { 0 };

	//基址 78380000
	__asm {
		push 0
		sub esp, 0x14
		mov ecx, esp
		push - 0x1
		push param1
		call call1
		sub esp, 0x14
		mov ecx, esp
		mov ebx, pathPtr
		push ebx
		call call2
		sub esp, 0x14
		mov ecx, esp
		push wxidPtr
		call call2
		lea eax, buff
		push eax
		call call3
		mov ecx, eax
		call call4
	}
}
