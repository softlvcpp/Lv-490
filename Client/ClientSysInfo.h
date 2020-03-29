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

#include <windows.h>
#include <malloc.h>    
#include <stdio.h>
#include <tchar.h>
#include<thread>

using namespace std;
namespace fs = std::filesystem;
class ClientSysInfo
{
	string OS;
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
	vector<int> HardDisk_Free;

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
	// Repace Calculate ->> Calcuate
	string CalculateOS_name();
	int CalculateRAM_Size();
	string CalculateCPU_vendor();
	//https docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation
	// Helper function to count set bits in the processor mask.	
	int CalculateCPU_cores();
	float CalculateCPU_Speed();
	std::vector<std::string> Calculatevector_logic_dick();
	QString getMacAddress();
	QString CalculateIPAddress();
	string CalculateHardDisk_MediaType(string LogicalDisk);
	int Calculatefree_space(std::string logic_drive);
	int Calculatecapacity(std::string logic_drive);
};

