#pragma once

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




//defines for Read()
#define CONFIG_ROOT "root"
#define CONFIG_SERVER "Server"
#define CONFIG_SERVERNAME "servername"
#define CONFIG_SERVERDISPLAYNAME "serverdisplayname"
#define CONFIG_LISTENPORT "listenport"
#define CONFIG_IPADRESS "ipadress"
#define CONFIG_COMMUNICATIONSETTINGS "communicationsetings"
#define CONFIG_BLOCKING "blocking"
#define CONFIG_SOCKET_TIMEOUT "socket_timeout"
#define CONFIG_LOGGIN "loggin"
#define CONFIG_FILENAME "filename"
#define CONFIG_LOGLEVEL "LogLevel"
#define CONFIG_FLUSH "flush"
#define CONFIG_TIME "time"
#define CONFIG_PERIOD_TIME "Period_time"
#define CONFIG_THREADPOOL "threadpool"
#define CONFIG_MAXWORKINGTHREAD "maxworkingthreads"

//defines for WriteSystemInformation() and PrepareToDBManager()
#define CLIENT_ROOT "root"
#define CLIENT_CLIENT "Client"
#define CLIENT_MACADRESS "MacAddress"
#define CLIENT_IPADRESS "IPAddress"
#define CLIENT_SYSTEMINFORMATION "SystemInformation"
#define CLIENT_HARDDISK "HardDisk"
#define CLIENT_TYPE "type"
#define CLIENT_DRIVE "drive"
#define CLIENT_HARDDISK_TOTALSIZE "TotalSize"
#define CLIENT_HARDDISK_USED "Used"
#define CLIENT_HARDDISK_FREE "Free"
#define CLIENT_TOTALRAM "TotalRAM"
#define CLIENT_CPU "CPU"
#define CLIENT_CPU_NUMBERS "CPUNumbers"
#define CLIENT_CPU_VENDOR "CPUVendor"
#define CLIENT_CPU_SPEED "CPUSpeed"