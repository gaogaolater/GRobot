#pragma once
#include <string>
using namespace std;

//�û�������Ϣ
struct MyInfo
{
	wchar_t wxid[0x200] = L"NULL";	//΢��ID
	wchar_t nickname[0x500] = L"NULL";	//�ǳ�
	wchar_t device[0x100] = L"NULL";	//��½�豸
	wchar_t phone[0x100] = L"NULL";	//���ֻ���
	wchar_t coutry[0x100] = L"NULL";	//����
	wchar_t city[0x100] = L"NULL";	//����
	wchar_t headImage[0x1000] = L"NULL";	//��ͷ��
};

MyInfo* GetMyInfo();
