// DatabaseManager.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "DatabaseManager.h"
#include <codecvt>
#include <locale>
using namespace std;

// This is an example of an exported variable
DATABASEMANAGER_API int nDatabaseManager=0;

// This is an example of an exported function.
DATABASEMANAGER_API int fnDatabaseManager(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CDatabaseManager::CDatabaseManager()
{
    return;
}

bool CDatabaseManager::connect()
{
	sqlConnHandle = NULL;
	m_sqlStmtHandle = NULL;
	if (!((SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_sqlEnvHandle)) && (SQL_SUCCESS == SQLSetEnvAttr(m_sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
		&& (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_DBC, m_sqlEnvHandle, &sqlConnHandle))))
		return false;

	switch (SQLDriverConnect(sqlConnHandle,
		NULL,
		(SQLWCHAR*)L"DRIVER={SQL Server};SERVER=ssu-sql12\\tc;DATABASE=Lv-490.C++;UID=Lv-490.C++;PWD=Lv-490.C++;",
		SQL_NTS,
		m_retconstring,
		1024,//
		NULL,
		SQL_DRIVER_NOPROMPT)) {

	case SQL_SUCCESS:
		return true;
	case SQL_SUCCESS_WITH_INFO:
		return true;
	case SQL_INVALID_HANDLE:
		return false;
	case SQL_ERROR:
		return false;
	default:
		return false;
	}

};

wstring CDatabaseManager::to_wstring(std::string str)
{
	using convert_t = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_t, wchar_t> strconverter;
	return strconverter.from_bytes(str);
}
bool CDatabaseManager::setClient(string macadress, string ip_adress,
	int total_ram, int cpu_number, int cpu_vendor,
	int cpu_speed, string os
	)
{
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &m_sqlStmtHandle))
	{
		return false;
	}
	using convert_t = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_t, wchar_t> strconverter;
	string query = "exec addClient '" + (macadress)+"', '";
	query += (ip_adress)+"', ";
	query += to_string((total_ram)) + ", " + to_string((cpu_number));
	query += ", " + to_string((cpu_vendor)) + ", ";
	query += to_string((cpu_speed)) + ", '" + (os)+"'; ";
	std::cout << query << std::endl;
	wstring qsql = to_wstring(query);
	return (SQL_SUCCESS == SQLExecDirect(m_sqlStmtHandle, (SQLWCHAR*)(qsql.c_str()), SQL_NTS));
};
bool CDatabaseManager::setDisk(string macadress, int diskmemory, string name)
{
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &m_sqlStmtHandle))
		return false;
	string query = "WITH m AS (SELECT top 1 client_id FROM client WHERE mac_adress = '";
	query += macadress + "')INSERT INTO disk VALUES((select TOP 1 client_id FROM m), ";
	query += to_string(diskmemory) + ", '" + name + "');";
	std::cout << query << std::endl;
	return (SQLExecDirect(m_sqlStmtHandle, (SQLWCHAR*)&query, SQL_NTS) == SQL_SUCCESS);
};
bool CDatabaseManager::createTables()
{
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &m_sqlStmtHandle))
		return false;
	SQLWCHAR* query = (SQLWCHAR*)&L"exec createTables;";
	return (SQLExecDirect(m_sqlStmtHandle, query, SQL_NTS) == SQL_SUCCESS);
}
bool CDatabaseManager::deleteTables()
{
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &m_sqlStmtHandle))
		return false;
	SQLWCHAR* query = (SQLWCHAR*)&L"exec deleteTables;";
	return (SQLExecDirect(m_sqlStmtHandle, query, SQL_NTS) == SQL_SUCCESS);
}