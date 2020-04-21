#pragma once
#pragma warning(disable:4251)
// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the XMLPARSER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// XMLPARSER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XMLPARSER_EXPORTS
#define XMLPARSER_API __declspec(dllexport)
#else
#define XMLPARSER_API __declspec(dllimport)
#endif

//config tags
extern const char g_CONFIG_ROOT[];
extern const char g_CONFIG_SERVER[];
extern const char g_CONFIG_SERVERNAME[];
extern const char g_CONFIG_SERVERDISPLAYNAME[];
extern const char g_CONFIG_LISTENPORT[];
extern const char g_CONFIG_IPADRESS[];
extern const char g_CONFIG_COMMUNICATIONSETTINGS[];
extern const char g_CONFIG_BLOCKING[];
extern const char g_CONFIG_SOCKET_TIMEOUT[];
extern const char g_CONFIG_LOGGIN[];
extern const char g_CONFIG_FILENAME[];
extern const char g_CONFIG_LOGLEVEL[];
extern const char g_CONFIG_FLUSH[];
extern const char g_CONFIG_TIME[];
extern const char g_CONFIG_PERIOD_TIME[];
extern const char g_CONFIG_THREADPOOL[];
extern const char g_CONFIG_MAXWORKINGTHREAD[];

//client tags
extern const char g_CLIENT_ROOT[];
extern const char g_CLIENT_CLIENT[];
extern const char g_CLIENT_OPERATION_SYSTEM[];
extern const char g_CLIENT_MACADRESS[];
extern const char g_CLIENT_IPADRESS[];
extern const char g_CLIENT_SYSTEMINFORMATION[];
extern const char g_CLIENT_HARDDISK[];
extern const char g_CLIENT_TYPE[];
extern const char g_CLIENT_DRIVE[];
extern const char g_CLIENT_HARDDISK_TOTALSIZE[];
extern const char g_CLIENT_HARDDISK_USED[];
extern const char g_CLIENT_HARDDISK_FREE[];
extern const char g_CLIENT_TOTALRAM[];
extern const char g_CLIENT_CPU[];
extern const char g_CLIENT_CPU_NUMBERS[];
extern const char g_CLIENT_CPU_VENDOR[];
extern const char g_CLIENT_CPU_SPEED[];
extern const char g_CLIENT_PROCESSINFORMATION[];
extern const char g_CLIENT_PROCESS[];
extern const char g_CLIENT_PROCESS_ATTRIBUTE_NAME[];
extern const char g_CLIENT_PROCESS_ATTRIBUTE_PID[];