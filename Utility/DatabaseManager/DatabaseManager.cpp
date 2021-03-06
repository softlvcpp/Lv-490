// DatabaseManager.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "DatabaseManager.h"
#include <codecvt>
#include <locale>
#include <time.h>    
using namespace std;
DatabaseMicrosoftSQLServer::DatabaseMicrosoftSQLServer()
{
	m_connection_string = ("DRIVER={SQL Server};SERVER=ssu-sql12\\tc;DATABASE=Lv-490.C++;UID=Lv-490.C++;PWD=Lv-490.C++;");
}

bool DatabaseMicrosoftSQLServer::Connect()
{
	//SQLAllocHandle in uniqueptr

	sql_conn_handle = NULL;
	m_sql_stmt_handle = NULL;
	if (!((SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_sql_env_handle)) && (SQL_SUCCESS == SQLSetEnvAttr(m_sql_env_handle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
		&& (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_DBC, m_sql_env_handle, &sql_conn_handle))))
		return false;
	wstring connection_string = to_wstring(m_connection_string);
	switch (SQLDriverConnect(sql_conn_handle,
		NULL,
		(SQLWCHAR*)(connection_string.c_str()),
		SQL_NTS,
		m_retconstring,
		1024,
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

void DatabaseMicrosoftSQLServer::Initialize(const string& server, const string& database, const string& uid, const string& password)
{
	m_connection_string = "DRIVER={SQL Server};SERVER=" + server + ";DATABASE=" + database + ";UID=" + uid + ";PWD=" + password + ";";
}
wstring DatabaseMicrosoftSQLServer::to_wstring(std::string str)
{
	using convert_t = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_t, wchar_t> strconverter;
	return strconverter.from_bytes(str);
}
bool DatabaseMicrosoftSQLServer::setClient(const string& macadress, const string& ip_adress,
	int total_ram, int cpu_number, int cpu_vendor, int cpu_speed, string os)
{
	string query = "exec addClient '" + (macadress)+"', '";
	query += (ip_adress)+"', ";
	query += to_string((total_ram)) + ", " + to_string((cpu_number));
	query += ", " + to_string((cpu_vendor)) + ", ";
	query += to_string((cpu_speed)) + ", '" + (os)+"'; ";
	return queryDatabase(query);
};
bool DatabaseMicrosoftSQLServer::queryDatabase(const string& query)
{
	cout << query << endl;
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sql_conn_handle, &m_sql_stmt_handle))
	{
		return false;
	}
	wstring qsql = to_wstring(query);
	return (SQL_SUCCESS == SQLExecDirect(m_sql_stmt_handle, (SQLWCHAR*)(qsql.c_str()), SQL_NTS));

};
string DatabaseMicrosoftSQLServer::DataBaseTime(time_t timer)
{
	struct tm timeinfo;
	char buffer[80];
	localtime_s(&timeinfo, &timer);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
	return buffer;
}
bool DatabaseMicrosoftSQLServer::setDisk(const string& macadress, const string& os, int memory, const string& disk_type, const string& disk_media_type, int total_size, int use, int free, time_t timer)
{
	string time_insert = DataBaseTime(timer);
	string query = "exec addDisk '" + macadress + "', '" + os + "', " + to_string(memory) + ", '";
	query += disk_type + "', '" + disk_media_type + "', " + to_string(total_size) + ", ";
	query += to_string(use) + ", " + to_string(free) + ", '" + time_insert + "';";
	return queryDatabase(query);
};
bool DatabaseMicrosoftSQLServer::setProcess(const string& macadress, const string& os, const string& process_name, int pid, time_t timer)
{
	string time_insert = DataBaseTime(timer);
	string query = "exec addProcess '" + macadress + "', '" + os + "', '" + process_name + "', ";
	query += to_string(pid) + ", '";
	query += time_insert + "';";
	return queryDatabase(query);
};

bool DatabaseMicrosoftSQLServer::CreateTables()
{
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sql_conn_handle, &m_sql_stmt_handle))
		return false;
	SQLWCHAR* query = (SQLWCHAR*)&L"exec createTables;";
	return (SQLExecDirect(m_sql_stmt_handle, query, SQL_NTS) == SQL_SUCCESS);
}
bool DatabaseMicrosoftSQLServer::DeleteTables()
{
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sql_conn_handle, &m_sql_stmt_handle))
		return false;
	SQLWCHAR* query = (SQLWCHAR*)&L"exec deleteTables;";
	return (SQLExecDirect(m_sql_stmt_handle, query, SQL_NTS) == SQL_SUCCESS);
}
bool DatabaseMicrosoftSQLServer::getClient()
{
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sql_conn_handle, &m_sql_stmt_handle))
		return false;
	if (SQL_SUCCESS != SQLExecDirect(m_sql_stmt_handle, (SQLWCHAR*)L"SELECT * from client", SQL_NTS)) {
		return false;
	}
	else 
	{
		SQLCHAR sqlVersion[10000];
		SQLLEN ptrSqlVersion;
		while (SQLFetch(m_sql_stmt_handle) == SQL_SUCCESS) {
			SQLGetData(m_sql_stmt_handle, 1, SQL_CHAR, sqlVersion, SQL_RESULT_LEN, &ptrSqlVersion);
			cout << sqlVersion << endl;
		}
	}
	return true;
}
DatabaseMicrosoftSQLServer::~DatabaseMicrosoftSQLServer() {
	SQLFreeHandle(SQL_HANDLE_STMT, m_sql_stmt_handle);
	SQLDisconnect(m_sql_stmt_handle);
	SQLFreeHandle(SQL_HANDLE_DBC, m_sql_stmt_handle);
	SQLFreeHandle(SQL_HANDLE_ENV, m_sql_env_handle);
}