#pragma once
#include <minwindef.h>
#include<list>
using namespace std;

struct DBNameHandle {
	int DBHandler;
	char DBName[MAX_PATH];
};

list<DBNameHandle> dbList;

void hookDBHandle();
