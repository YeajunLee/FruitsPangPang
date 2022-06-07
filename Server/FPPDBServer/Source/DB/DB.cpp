#include <string>
#include "DB.h"

SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt;

using namespace std;

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	cout << "�׸�������" << endl;
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	auto t = SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL);

	if (t == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

void InitializeDB()
{
	SQLRETURN retcode;
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)15, 0);	//Ÿ�Ӿƿ��ð� 15�ʷ� ����

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"FPP_DBServer", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					cout << "connection success!" << endl;

					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				}
				else {
					HandleDiagnosticRecord(hstmt, SQL_HANDLE_DBC, retcode);
					cout << "connection fail!" << endl;
				}
			}
		}
	}
}

void ReleaseDB()
{
	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}



int Login(const char* name, const char* password, LoginInfo& p_info)
{
	SQLINTEGER p_coin{};
	SQLSMALLINT p_skintype{};
	SQLWCHAR p_name[21]{}, p_password[21]{};
	SQLLEN cbName = 0, cbPassword = 0, cbP_coin = 0, cbP_skintype = 0;
	SQLRETURN retcode{};

	//cout << "ODBC Connected !" << endl;
	wstring LoginQuery{ L"EXEC try_login " };
	USES_CONVERSION;
	LoginQuery += A2W(name);
	LoginQuery += L",";
	LoginQuery += A2W(password);

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)LoginQuery.c_str(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {


		// Bind columns 1, 2, and 3  
		// mssql���� varchar(10) �� ���� ��, db���� 10���� ���� ������ ����.
		// c++�� �ܾ�ö� �� 10���ڴ� 1���ڴ� 2byte�� ġȯ�Ǿ sqlbindcol�� bufferlen���� 20byte + ���ڿ��� 2byte  �� 22byte�� �޾ƾ� ��.
		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, p_name, 42, &cbName);			//wchar�� �ѱ��ڴ� 2byte�̹Ƿ� 10���ڿ��� 20 + 2(���ڿ� ��)�� �ʿ�
		retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, p_password, 42, &cbPassword);
		retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &p_coin, 100, &cbP_coin);
		retcode = SQLBindCol(hstmt, 4, SQL_C_SHORT, &p_skintype, 100, &cbP_skintype);

		// Fetch and print each row of data. On an error, display a message and exit.  
		{
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				WideCharToMultiByte(CP_ACP, 0, p_name, -1, p_info.p_name, 21, 0, 0);
				WideCharToMultiByte(CP_ACP, 0, p_password, -1, p_info.p_password, 21, 0, 0);
				p_info.p_coin = p_coin;
				p_info.p_skintype = p_skintype;

				SQLCancel(hstmt);

				if (0 == strcmp(p_info.p_password, password))
				{
					return true;
				}

				return -3;	//��й�ȣ�� �ٸ�
			}
			else if (retcode == SQL_ERROR) {
				HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
				SQLCancel(hstmt);
				return -2;
			}
		}

	}
	else {
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
		// Process data  
		SQLCancel(hstmt);
		return -2;
	}

	// Process data  
	SQLCancel(hstmt);
	return -2;
}


int SignUp(const char* name, const char* password)
{
	SQLINTEGER p_coin{};
	SQLSMALLINT p_skintype{};
	SQLWCHAR p_name[21]{}, p_password[21]{};
	SQLLEN cbName = 0, cbPassword = 0, cbP_coin = 0, cbP_skintype = 0;
	SQLRETURN retcode{};

	wstring SignUpQuery{ L"EXEC make_character " };
	wstring FindCharacterQuery{ L"EXEC find_character " };
	USES_CONVERSION;
	SignUpQuery += A2W(name);
	SignUpQuery += L",";
	SignUpQuery += A2W(password);

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)SignUpQuery.c_str(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		return 1;
	}
	else if (retcode == SQL_ERROR) {

		FindCharacterQuery += A2W(name);

		retcode = SQLExecDirect(hstmt, (SQLWCHAR*)FindCharacterQuery.c_str(), SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, p_name, 42, &cbName);			//wchar�� �ѱ��ڴ� 2byte�̹Ƿ� 10���ڿ��� 20 + 2(���ڿ� ��)�� �ʿ�
			retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, p_password, 42, &cbPassword);
			retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &p_coin, 100, &cbP_coin);
			retcode = SQLBindCol(hstmt, 4, SQL_C_SHORT, &p_skintype, 100, &cbP_skintype);

			retcode = SQLFetch(hstmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLCancel(hstmt);
				return -4;	// �ߺ����̵� ����.
			}
			else if (retcode == SQL_ERROR) {
				SQLCancel(hstmt);
				return -1; // �� �� ���� ������ ����
			}
		}

		return -1;	// �� �� ���� ������ ����
	}
}
