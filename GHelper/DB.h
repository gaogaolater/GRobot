#pragma once
#include <minwindef.h>
#include<list>
#include <string>

using namespace std;

typedef int (*sqlite3_callback)(void*, int, char**, char**);
typedef int (*sqlite3_exec)(
    DWORD,                                  /* An open database */
    const char*,                            /* SQL to be evaluated */
    sqlite3_callback,                       /* Callback function */
    void*,                                  /* 1st argument to callback */
    char**                                  /* Error msg written here */
);

struct DBNameHandle {
	int DBHandler;
	char DBName[MAX_PATH];
};
void HookDBHandle();
list<DBNameHandle> GetDbListHandle();
int ExecSql(string dbName, string sql, sqlite3_callback callback, char*& err);