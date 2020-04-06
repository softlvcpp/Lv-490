#pragma once

#include <QApplication>
#include <iostream>
#include <QStorageInfo>
#include <qdebug.h>
#include <QList>
#include <thread>
#include <array>
#include <vector>
#include <atlbase.h>
#include <atlstr.h>
#include <comutil.h>
#include <wbemidl.h>
#include <string>
#include<qnetworkinterface.h>
#include<qhostaddress.h>
#include<filesystem>
#include<map>

#include <windows.h>
#include <malloc.h>    
#include <stdio.h>
#include <tchar.h>
#include<thread>

#include "../Utility/XML_Parser/XML_Parser.h"

using namespace std;
namespace fs = std::filesystem;
class ClientSysInfo
{
	/*string OS;
	string MacAddress;
	string IPAddress;
	int TotalRAM;
	int CPUNumbers;
	string CPUVendor;
	int CPUSpeed;
	vector<string> HardDisk_type_list;
	vector<string> HardDisk_MediaType;
	vector<int> HardDisk_TotalSize;
	vector<int> HardDisk_Used;
	vector<int> HardDisk_Free;*/
	CXMLParser::ClientInfo m_client_info;
	std::map <int,string> m_processes;
public:
	ClientSysInfo();
	void Update();
	string get_OS();
	string get_MacAddress();
	string get_IPAddress();
	int get_TotalRAM();
	int get_CPUNumbers();
	string  get_CPUVendor();
	int  get_CPUSpeed();
	vector<string>  get_HardDisk_type_list();
	vector<string>  get_HardDisk_MediaType();
	vector<int>  get_HardDisk_TotalSize();
	vector<int>  get_HardDisk_Used();
	vector<int>  get_HardDisk_Free();
	map<int, string> get_Processes();

	string CalculateOS();
	int CalculateTotalRAM();
	string CalculateCPUVendor();
	//https docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation
	// Helper function to count set bits in the processor mask.	
	int CalculateCPUNumbers();
	float CalculateCPUSpeed();
	std::vector<std::string> CalculatevectorLogicDick();
	string CalculateHardDisk_MediaType(string LogicalDisk);
	int CalculateFreeSpace(const std::string &logic_drive);
	int CalculateCapacity(const std::string &logic_drive);
	QString CalculateMacAddress();
	QString CalculateIPAddress();
	void CalculateProcesses();
	//ui
	//socket
	~ClientSysInfo();
public: 
	CXMLParser::ClientInfo get_client_info() const;
};

