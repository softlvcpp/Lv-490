#pragma once
// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the DATABASEMANAGER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// DATABASEMANAGER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DATABASEMANAGER_EXPORTS
#define DATABASEMANAGER_API __declspec(dllexport)
#else
#define DATABASEMANAGER_API __declspec(dllimport)
#endif
#include <string>
#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <iomanip>
#include <ctime>
using namespace std;

class DATABASEMANAGER_API DatabaseManager {
public:
	//SQLServerManager		 factory(abstract)
	DatabaseManager();
	bool Connect();//
	bool CreateTables();//
	bool DeleteTables();//
	bool setClient(const string& macadress, const string& ip_adress, int total_ram, int cpu_number, int cpu_vendor, int cpu_speed, string os);
	bool setDisk(const string& macadress, const string& os, int memory, const string& disk_type, const string& disk_media_type, int total_size, int use, int free, time_t timer);
	string DataBaseTime(time_t timer);
	void Initialize(const string& server, const string& database, const string& uid, const string& password);//
private:
	const int SQL_RESULT_LEN = 240;
	const int SQL_RETURN_CODE_LEN = 1000;
	string m_connection_string;
	bool queryDatabase(string query);
	wstring to_wstring(std::string wstr);
	SQLHANDLE sql_conn_handle;
	SQLHANDLE m_sql_stmt_handle;
	SQLHANDLE m_sql_env_handle;
	SQLWCHAR m_retconstring[1000];
};

extern DATABASEMANAGER_API int nDatabaseManager;

DATABASEMANAGER_API int fnDatabaseManager(void);
