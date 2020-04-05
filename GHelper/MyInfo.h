#pragma once
#include <string>
using namespace std;

//用户个人信息
struct MyInfo
{
	wchar_t wxid[0x200] = L"NULL";	//微信ID
	wchar_t nickname[0x500] = L"NULL";	//昵称
	wchar_t device[0x100] = L"NULL";	//登陆设备
	wchar_t phone[0x100] = L"NULL";	//绑定手机号
	wchar_t coutry[0x100] = L"NULL";	//国籍
	wchar_t city[0x100] = L"NULL";	//城市
	wchar_t headImage[0x1000] = L"NULL";	//大头像
};

MyInfo* GetMyInfo();
