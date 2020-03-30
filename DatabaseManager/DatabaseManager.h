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
using namespace std;
// This class is exported from the dll
class DATABASEMANAGER_API CDatabaseManager {
public:
	CDatabaseManager(void);
	
		const int SQL_RESULT_LEN = 240;//conventions
		const int SQL_RETURN_CODE_LEN = 1000;
		bool connect();
		bool createTables();
		bool deleteTables();
		bool setClient(string macadress, string ip_adress, int total_ram, int cpu_number, int cpu_vendor, int cpu_speed, string os);
		bool setDisk(string macadress, int disk_memory, string name);
		SQLHANDLE sqlConnHandle;
		//bool initialize()
		wstring to_wstring(std::string wstr);
		SQLHANDLE m_sqlStmtHandle;
		SQLHANDLE m_sqlEnvHandle;
		SQLWCHAR m_retconstring[1000];
	

	// TODO: add your methods here.
};

extern DATABASEMANAGER_API int nDatabaseManager;

DATABASEMANAGER_API int fnDatabaseManager(void);
