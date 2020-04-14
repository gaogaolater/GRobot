#include "MyInfo.h"
#include "Utils.h"
#include <corecrt_wstdio.h>
#include "offset.h"
#include <tchar.h>

MyInfo * GetMyInfo()
{
	/*
	* 个人信息
	* 昵称：16187EC		字符或指针
	* 手机：‭1618821‬		字符
	* 城市：16188d9		字符
	* 区域：16188f1		字符
	* 手机类型：1618c28	字符
	* 国家：16189c8		字符
	* Wxid：1618774		指针
	* 头像：1618ab4  ‭1618ACC	指针
	‬*/
	MyInfo* info = new MyInfo;
	swprintf_s(info->city, L"%S", (char*)(GetWeChatWinAddress() + OF_CITY));
	swprintf_s(info->coutry, L"%S", (char*)(GetWeChatWinAddress() + OF_COUNTRY));
	swprintf_s(info->device, L"%S", (char*)(GetWeChatWinAddress() + OF_DEVICE));
	swprintf_s(info->nickname, L"%S", (char*)(GetWeChatWinAddress() + OF_NICKNAME));
	swprintf_s(info->phone, L"%S", (char*)(GetWeChatWinAddress() + OF_PHONE));
	swprintf_s(info->wxid, L"%S", (char*)(*(DWORD*)(GetWeChatWinAddress() + OF_WXID)));
	swprintf_s(info->device, L"%S", (char*)(GetWeChatWinAddress() + OF_DEVICE));
	swprintf_s(info->headImage, L"%S", (char*)(*(DWORD*)(GetWeChatWinAddress() + OF_HEADIMAGE)));
	return info;
}


//Json::Value GetMyInfoJson()
//{
//	MyInfo* info = GetMyInfo();
//	Json::Value root;
//	root["city"] = info->city;
//	root["coutry"] = info->coutry;
//	root["device"] = info->device;
//	root["nickname"] = info->nickname;
//	root["phone"] = info->phone;
//	root["wxid"] = info->wxid;
//	root["device"] = info->device;
//	root["headImage"] = info->headImage;
//	return root;
//}


void getContactList()
{
	//ListView_DeleteAllItems(GetDlgItem(getGlobalHwnd(), FRIEND_LIST));
	char* sqlErrmsg = NULL;
	//runSql("MicroMsg.db", "select Contact.UserName,Contact.Alias,Contact.NickName,Contact.Remark,ContactHeadImgUrl.bigHeadImgUrl from Contact LEFT OUTER JOIN ContactHeadImgUrl ON Contact.UserName = ContactHeadImgUrl.usrName", getContactCallBack, sqlErrmsg);
}