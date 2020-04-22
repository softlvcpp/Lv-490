#pragma once
#include <iostream>
#include <thread>
#include <string>
#include <array>
#include<map>
#include<filesystem>
#include <vector>
#include <atlbase.h>
#include <atlstr.h>
#include <comutil.h>
#include <wbemidl.h>


#include <windows.h>
#include <malloc.h>    
#include <stdio.h>
#include <tchar.h>

#include "../Utility/XML_Parser/XMLClient.h"

using namespace std;
namespace fs = std::filesystem;






class ClientSysInfo
{
	XMLClient m_client_info;
public:
	ClientSysInfo();
	void Update(); 
	void Parse(string& xml_str) const noexcept;
	
	string  get_os()const;
	string  get_mac_address()const;
	string  get_ip_address()const;

	int  get_total_ram()const;
	int  get_cpu_numbers()const;
	int  get_cpu_speed()const;

	string  get_cpu_vendor()const;

	vector<string>   get_hard_disk_type_list()const;
	vector<string>   get_hard_disk_media_type()const;
	vector<int>      get_hard_disk_total_size()const;
	vector<int>      get_hard_disk_used()const;
	vector<int>      get_hard_disk_free()const;

	map <int, string> get_processes()const;
	
	void CalculateProcesses();
private:
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
	string CalculateMacAddress();
	string CalculateIPAddress();

};

