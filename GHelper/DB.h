#pragma once
#include <minwindef.h>
#include<list>
using namespace std;

struct DBNameHandle {
	int DBHandler;
	char DBName[MAX_PATH];
};
void hookDBHandle();
list<DBNameHandle>* GetDbListHandle();

