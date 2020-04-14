#include "ClientSysInfo.h"
#include "DefineLogger.h"
#include <tlhelp32.h> // for snapshot

//qt libs
#include <QApplication>
#include <QStorageInfo>
#include <qdebug.h>
#include <QList>
#include<qnetworkinterface.h>
#include<qhostaddress.h>
////
#include <windows.h>
#include <conio.h>
//#include <boost/thread.hpp>
ClientSysInfo::ClientSysInfo() {
	Update();
}

void  ClientSysInfo::Update() {

	m_client_info.set_os(CalculateOS());
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	//qDebug() << systemInfo.dwNumberOfProcessors;
	string os_name = QSysInfo::productType().toStdString();

	if (os_name == "windows") {
		m_client_info.set_total_ram(CalculateTotalRAM());
		m_client_info.set_cpu_numbers(CalculateCPUNumbers());
		m_client_info.set_cpu_vendor(CalculateCPUVendor());
		m_client_info.set_cpu_speed(CalculateCPUSpeed());
		vector<string> hard_disk_type_list = CalculatevectorLogicDick();
		m_client_info.set_hard_disk_type_list(hard_disk_type_list);
		/*m_client_info.HardDisk_TotalSize.clear();
		m_client_info.HardDisk_Free.clear();
		m_client_info.HardDisk_Used.clear();
		m_client_info.HardDisk_MediaType.clear();*/
		vector<string>  hard_disk_media_type;
		vector<int>     hard_disk_total_size;
		vector<int>     hard_disk_used;
		vector<int>     hard_disk_free;

		if (hard_disk_type_list.size() != 0) {
			for (int i = 0; i < hard_disk_type_list.size(); i++) {
				hard_disk_total_size.push_back(CalculateCapacity(hard_disk_type_list[i]));
				hard_disk_free.push_back(CalculateFreeSpace(hard_disk_type_list[i]));
				hard_disk_used.push_back(hard_disk_total_size[i] - hard_disk_free[i]);
				hard_disk_media_type.push_back(CalculateHardDisk_MediaType(hard_disk_type_list[i]));
				qDebug() << i;
			}
		}
		m_client_info.set_hard_disk_total_size(hard_disk_total_size);
		m_client_info.set_hard_disk_free(hard_disk_free);
		m_client_info.set_hard_disk_used(hard_disk_used);
		m_client_info.set_hard_disk_media_type(hard_disk_media_type);
		m_client_info.set_mac_address(CalculateMacAddress());
		m_client_info.set_ip_address(CalculateIPAddress());
	}
	else if (os_name == "linux") {} // For the future :)
	else {
		//undefined m_client_info.OS;
		//TODO logger error
	}
}

void ClientSysInfo::Parse(string& xml_str) const noexcept
{
	m_client_info.WriteSystemInformation(xml_str);
	
}



string ClientSysInfo::get_os() const
{
	return m_client_info.get_os();
}

string ClientSysInfo::get_mac_address() const
{
	return m_client_info.get_mac_address();
}

string ClientSysInfo::get_ip_address() const
{
	return m_client_info.get_ip_address();
}

int ClientSysInfo::get_total_ram() const
{
	return m_client_info.get_total_ram();
}

int ClientSysInfo::get_cpu_numbers() const
{
	return m_client_info.get_cpu_numbers();
}

int ClientSysInfo::get_cpu_speed() const
{
	return m_client_info.get_cpu_speed();
}

string ClientSysInfo::get_cpu_vendor() const
{
	return m_client_info.get_cpu_vendor();
}

vector<string> ClientSysInfo::get_hard_disk_type_list() const
{
	return m_client_info.get_hard_disk_type_list();
}

vector<string> ClientSysInfo::get_hard_disk_media_type() const
{
	return m_client_info.get_hard_disk_media_type();
}

vector<int> ClientSysInfo::get_hard_disk_total_size() const
{
	return m_client_info.get_hard_disk_total_size();
}

vector<int> ClientSysInfo::get_hard_disk_used() const
{
	return m_client_info.get_hard_disk_used();
}

vector<int> ClientSysInfo::get_hard_disk_free() const
{
	return m_client_info.get_hard_disk_free();
}

map<int, string> ClientSysInfo::get_processes() const
{
	return m_client_info.get_processes();
}





//int ClientSysInfo::get_TotalRAM() { return m_client_info.TotalRAM; }
//string  ClientSysInfo::get_CPUVendor() { return m_client_info.CPUVendor; }
//int  ClientSysInfo::get_CPUSpeed() { return m_client_info.CPUSpeed; }
//vector<string>  ClientSysInfo::get_HardDisk_MediaType() { return m_client_info.HardDisk_MediaType; }
//vector<int>  ClientSysInfo::get_HardDisk_TotalSize() { return m_client_info.HardDisk_TotalSize; }
//vector<int>  ClientSysInfo::get_HardDisk_Used() { return m_client_info.HardDisk_Used; }
//vector<int>  ClientSysInfo::get_HardDisk_Free() { return m_client_info.HardDisk_Free; }
////bodya
//
//string ClientSysInfo::get_OS() { return m_client_info.OS; }
//string ClientSysInfo::get_MacAddress() { return m_client_info.MacAddress; }
//string ClientSysInfo::get_IPAddress() { return m_client_info.IPAddress; }
//vector<string>  ClientSysInfo::get_HardDisk_type_list() { return m_client_info.HardDisk_type_list; }
//int ClientSysInfo::get_CPUNumbers() { return m_client_info.CPUNumbers; }
//soketu


string ClientSysInfo::CalculateOS() {
	QString OSName = QSysInfo::prettyProductName();
	return OSName.toStdString();
}

int ClientSysInfo::CalculateTotalRAM() {

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	int RAM_size = statex.ullTotalPhys / 1024 / 1024;//MB
	return RAM_size;
}

string ClientSysInfo::CalculateCPUVendor() {
	std::vector<std::array<int, 4>> data_;
	std::array<int, 4> cpui;
	int nIds_;
	int nExIds_;
	std::string vendor_;
	std::string brand_;
	// Calling __cpuid with 0x0 as the function_id argument
	// gets the number of the highest valid function ID.
	__cpuid(cpui.data(), 0);
	nIds_ = cpui[0];

	for (int i = 0; i <= nIds_; ++i)
	{
		__cpuidex(cpui.data(), i, 0);
		data_.push_back(cpui);
	}
	// Capture vendor string
	char vendor[0x20];
	memset(vendor, 0, sizeof(vendor));
	*reinterpret_cast<int*>(vendor) = data_[0][1];
	*reinterpret_cast<int*>(vendor + 4) = data_[0][3];
	*reinterpret_cast<int*>(vendor + 8) = data_[0][2];

	return vendor;
}

float ClientSysInfo::CalculateCPUSpeed()
{
	LARGE_INTEGER qwWait, qwStart, qwCurrent;
	QueryPerformanceCounter(&qwStart);
	QueryPerformanceFrequency(&qwWait);
	qwWait.QuadPart >>= 5;
	unsigned __int64 Start = __rdtsc();
	do
	{
		QueryPerformanceCounter(&qwCurrent);
	} while (qwCurrent.QuadPart - qwStart.QuadPart < qwWait.QuadPart);
	float res = ((__rdtsc() - Start) << 5) / 1000000.0;
	return res;
}

std::vector<std::string> ClientSysInfo::CalculatevectorLogicDick() {
	std::vector<std::string> disks;
	QList listVolumes = QStorageInfo::mountedVolumes();
	for (int i = 0; i < listVolumes.size(); i++) {
		if (listVolumes[i].isValid() && listVolumes[i].isReady()) {
			if (!listVolumes[i].isReadOnly()) {
				disks.push_back(listVolumes[i].rootPath().toStdString());
			}
		}
	}
	return disks;
}




std::string ClientSysInfo::CalculateMacAddress()
{
	QNetworkInterface res;
	QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
	for (size_t i = 0; i < list.size(); i++) {
		if (list[i].type() == QNetworkInterface::Ethernet || list[i].type() == QNetworkInterface::Wifi)
		{
			if (list[i].flags().testFlag(QNetworkInterface::IsRunning) && list[i].humanReadableName() == "Wi-Fi")
			{
				res = list[i];
				break;
			}
			else if (list[i].humanReadableName() == "Ethernet")
			{
				res = list[i];
			}
		}
	}
	return res.hardwareAddress().toStdString();
}

std::string ClientSysInfo::CalculateIPAddress()
{
	QNetworkInterface mac_interface = QNetworkInterface::interfaceFromName(CalculateMacAddress().c_str());
	QList<QHostAddress> list_ip = mac_interface.allAddresses();
	QString res;
	for (size_t i = 0; i < list_ip.size(); i++)
	{
		if (!list_ip[i].isLinkLocal() && !list_ip[i].isLoopback()) {
			res = list_ip[i].toString();
		}
	}
	return res.toStdString();
}



string ClientSysInfo::CalculateHardDisk_MediaType(string LogicalDisk) { // "C:/" is input paramenter 
	// wstring  str1 = L"\\\\.\\C:"; parameter must be like this 
	LogicalDisk.insert(0, std::string("\\\\.\\"));
	LogicalDisk.erase(LogicalDisk.end() - 1);
	//qDebug() << "LogicalDisk driver" << QString(LogicalDisk.c_str());
	std::wstring stemp = std::wstring(LogicalDisk.begin(), LogicalDisk.end());
	LPCWSTR wszDrive = stemp.c_str();
	HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined 
	BOOL bResult = FALSE;                 // results flag
	DWORD junk = 0;                     // discard results

	hDevice = CreateFileW(wszDrive,          // drive to open
		0,                // no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE,
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attributes

	if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
	{
		return (FALSE);
	}
	DISK_GEOMETRY pdg = { 0 }; // disk drive geometry structure
	bResult = DeviceIoControl(hDevice,                       // device to be queried
		IOCTL_DISK_GET_DRIVE_GEOMETRY, // operation to perform
		NULL, 0,                       // no input buffer
		&pdg, sizeof(pdg),            // output buffer
		&junk,                         // # bytes returned
		(LPOVERLAPPED)NULL);          // synchronous I/O

	string string_res;
	STORAGE_PROPERTY_QUERY spqSeekP;
	spqSeekP.PropertyId = (STORAGE_PROPERTY_ID)StorageDeviceSeekPenaltyProperty;
	spqSeekP.QueryType = PropertyStandardQuery;

	DWORD bytesReturned = 0;
	DEVICE_SEEK_PENALTY_DESCRIPTOR dspd = { 0 };
	if (::DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &spqSeekP, sizeof(spqSeekP), &dspd, sizeof(dspd), &bytesReturned, NULL) &&
		bytesReturned == sizeof(dspd))
	{
		if (dspd.IncursSeekPenalty) {
			string_res = "HDD"; //seekPenalty = Y-- - HDD
		}
		else {
			string_res = "SSD";//seekPenalty=N---SSD
		}
	}
	else
	{
		cout << "err";
	}

	CloseHandle(hDevice);
	return string_res;
}




//std::map <int, string> m_processes;


typedef BOOL(WINAPI* LPFN_GLPI)(
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
	PDWORD);

int ClientSysInfo::CalculateCPUNumbers() {
	//return boost::thread::physical_concurrency(); if have boost
	LPFN_GLPI glpi;
	BOOL done = FALSE;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	DWORD returnLength = 0;
	DWORD processorCoreCount = 0;
	DWORD byteOffset = 0;
	PCACHE_DESCRIPTOR Cache;
	glpi = (LPFN_GLPI)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),
		"GetLogicalProcessorInformation");
	if (NULL == glpi)
	{
		L_TRACE << ("\nGetLogicalProcessorInformation is not supported.\n");
		return (1);
	}

	while (!done)
	{
		DWORD rc = glpi(buffer, &returnLength);

		if (FALSE == rc)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer)
					free(buffer);

				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
					returnLength);

				if (NULL == buffer)
				{
					L_TRACE << "\nError: Allocation failure\n";
					return (2);
				}
			}
			else
			{
				L_TRACE << "\nError %d" + GetLastError();
				return (3);
			}
		}
		else
		{
			done = TRUE;
		}
	}

	ptr = buffer;

	while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
	{
		switch (ptr->Relationship)
		{

		case RelationProcessorCore:
			processorCoreCount++;
			break;

		default:
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}
	return processorCoreCount;
}


int ClientSysInfo::CalculateFreeSpace(const std::string& logic_drive) {
	fs::space_info tmpi = fs::space(logic_drive);
	int size_in_GB = tmpi.free / 1024 / 1024 / 1024;
	return size_in_GB;
}

int ClientSysInfo::CalculateCapacity(const std::string& logic_drive) {
	fs::space_info tmpi = fs::space(logic_drive);
	int size_in_GB = tmpi.capacity / 1024 / 1024 / 1024;
	return size_in_GB;
}

void ClientSysInfo::CalculateProcesses() {
	HANDLE Snapshot_handle;
	Snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Snapshot_handle == NULL)
	{
		cout << "error" << endl;// in logger	
	}
	PROCESSENTRY32 proc;
	proc.dwSize = sizeof(PROCESSENTRY32);
	//m_processes.clear();
	map<int, string> processes;
	if (Process32First(Snapshot_handle, &proc))//Returns TRUE if the first entry of the process list has been copied to the buffer or FALSE otherwise.
	{
		wstring ws(proc.szExeFile);
		string str(ws.begin(), ws.end());
		processes.insert(pair<int, string>(proc.th32ProcessID, str));
		while (Process32Next(Snapshot_handle, &proc)) {
			wstring ws(proc.szExeFile);
			string str(ws.begin(), ws.end());
			//cout << str << endl;
			//qDebug() << "NAME:" + QString(str.c_str()) + "  ID: " + QString::number(proc.th32ProcessID);
			processes.insert(pair<int, string>(proc.th32ProcessID, str));
		}
	}

	
	CloseHandle(Snapshot_handle);
	m_client_info.set_processes(processes);
}

//ClientSysInfo::~ClientSysInfo()
//{
//
//}


