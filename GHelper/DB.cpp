#include "DB.h"
#include "Utils.h"
#include "resource.h"
#include <string>
#include "pch.h"
DWORD dbHookAddress;
std::wstring dbStrAll;
list<DBNameHandle> dbList;

void SetDBItem(int path, int dbHandle) {
	DBNameHandle item = {0};
	item.DBHandler = dbHandle;
	sprintf_s(item.DBName, "%s", (char*)path);
	dbList.push_back(item);
	dbStrAll += to_wstring(dbHandle);
	dbStrAll += L"-";
	dbStrAll += CharToWchar((char*)path);
	dbStrAll += L"\r\n";
	SetDlgItemText(getGlobalHwnd(), IDC_LOG, dbStrAll.c_str());
}

__declspec(naked) void My_Hook()
{
	__asm
	{
		mov esi, dword ptr ss : [ebp - 0x14]
		add esp, 0x8
		pushad
		//第二个参数 数据库句柄
		push[ebp - 0x14]
		//第一个参数 数据库路径
		push[ebp - 0x24]
		call SetDBItem
		add esp, 8
		popad
		jmp dbHookAddress
	}
}

void hookDBHandle()
{
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
	/*7935F8F2    B9 346C247A     mov ecx, WeChatWi.7A246C34
	7935F8F7    6A 00           push 0x0
	7935F8F9    6A 06           push 0x6
	7935F8FB    8D55 EC         lea edx, dword ptr ss : [ebp - 0x14]
	7935F8FE    E8 2D204700     call WeChatWi.797D1930
	7935F903    8B75 EC         mov esi, dword ptr ss : [ebp - 0x14]
	7935F906    83C4 08         add esp, 0x8
	base 78EE0000
	offset ‭47F903
	*/
	DWORD hookAddress = dllAddr + 0x47F903;
	dbHookAddress = hookAddress + 6;
	BYTE jumpCode[6] = {0};
	jumpCode[0] = 0xE9;//jump
	jumpCode[5] = 0x90;//nop
	*(DWORD*)&jumpCode[1] = (DWORD)My_Hook - hookAddress - 5;
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookAddress, jumpCode, 6, NULL);
}

list<DBNameHandle>* GetDbListHandle()
{
	return &dbList;
}


