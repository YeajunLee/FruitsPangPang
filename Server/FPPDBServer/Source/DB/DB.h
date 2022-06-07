#pragma once
#include <iostream>
#include "../Game/Network/Network.h"
#include <sqlext.h>
#include <atlconv.h>


extern SQLHENV henv;
extern SQLHDBC hdbc;
extern SQLHSTMT hstmt;
void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

void InitializeDB();
void ReleaseDB();
int Login(const char* name, const char* password, LoginInfo& p_info);
int MakeCharacter(const char* name, const char* password);