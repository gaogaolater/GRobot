#include "DB.h"
#include "Utils.h"

void hookDBHandle()
{
	while (true) {
		DWORD dllAddr = GetWeChatWinAddress();
		if (dllAddr == 0) {
			Sleep(100);
		}
		else {
			break;
		}
	}
	
}
