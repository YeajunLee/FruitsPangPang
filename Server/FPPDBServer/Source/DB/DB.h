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
int SignUp(const char* name, const char* password);
int GetShopData(dl_packet_getiteminfo& shopInfo);
int BuyItem(const char* name, const char& itemcode, const int& coin);
int EquipItem(const char* name, const short& skintype);
int GetPlayerInfo(const char* name, LoginInfo& p_info);
int UpdatePlayerInfo(const char* name, const int& p_info);
int DailyReward(const char* name, LoginInfo& p_info);