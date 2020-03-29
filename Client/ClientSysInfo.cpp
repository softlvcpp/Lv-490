#include "ClientSysInfo.h"
#include<qthread.h>
ClientSysInfo::ClientSysInfo() {
	Update();
}

void  ClientSysInfo::Update() {
	OS = CalculateOS_name();
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	qDebug()<< systemInfo.dwNumberOfProcessors;;
	if (OS == "windows") {
		TotalRAM = CalculateRAM_Size();
		CPUNumbers = CalculateCPU_cores();
		CPUVendor = CalculateCPU_vendor();
		CPUSpeed = CalculateCPU_Speed();
		HardDisk_type_list = Calculatevector_logic_dick();
		if (HardDisk_type_list.size() != 0) {
			for (int i = 0; i < HardDisk_type_list.size(); i++) {
				HardDisk_TotalSize.push_back(Calculatecapacity(HardDisk_type_list[i]));
				HardDisk_Free.push_back(Calculatefree_space(HardDisk_type_list[i]));
				HardDisk_Used.push_back(HardDisk_TotalSize[i] - HardDisk_Free[i]);
				HardDisk_MediaType.push_back(CalculateHardDisk_MediaType(HardDisk_type_list[i]));
			}
		}
		MacAddress = getMacAddress().toStdString();
		IPAddress = CalculateIPAddress().toStdString();
	}
	else if (OS == "linux") {} // For the future :)
	else {
		//undefined OS;
		//TODO logger error
	}
}

int ClientSysInfo::get_TotalRAM() { return TotalRAM; }
string  ClientSysInfo::get_CPUVendor() { return CPUVendor; }
int  ClientSysInfo::get_CPUSpeed() { return CPUSpeed; }
vector<string>  ClientSysInfo::get_HardDisk_MediaType() { return HardDisk_MediaType; }
vector<int>  ClientSysInfo::get_HardDisk_TotalSize() { return HardDisk_TotalSize; }
vector<int>  ClientSysInfo::get_HardDisk_Used() { return HardDisk_Used; }
vector<int>  ClientSysInfo::get_HardDisk_Free() { return HardDisk_Free; }
//bodya

string ClientSysInfo::get_OS() { return OS; }
string ClientSysInfo::get_MacAddress() { return MacAddress; }
string ClientSysInfo::get_IPAddress() { return IPAddress; }
vector<string>  ClientSysInfo::get_HardDisk_type_list() { return HardDisk_type_list; }
int ClientSysInfo::get_CPUNumbers() { return CPUNumbers; }
//soketu


string ClientSysInfo::CalculateOS_name() {
	QString OSName = QSysInfo::productType();
	return OSName.toStdString();
}

int ClientSysInfo::CalculateRAM_Size() {

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	int RAM_size = statex.ullTotalPhys / 1024 / 1024;//MB
	return RAM_size;
}

string ClientSysInfo::CalculateCPU_vendor() {
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

float ClientSysInfo::CalculateCPU_Speed()
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

std::vector<std::string> ClientSysInfo::Calculatevector_logic_dick() {
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




QString ClientSysInfo::getMacAddress()
{
	QNetworkInterface res;
	QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
	for (int i = 0; i < list.size(); i++) {
		if (!(list[i].flags() & QNetworkInterface::IsLoopBack) && (list[i].type() == 3 || list[i].type() == 8)
			&& list[i].flags().testFlag(QNetworkInterface::IsRunning) && (list[i].humanReadableName() == "Ethernet" || list[i].humanReadableName() == "Wi-Fi"))
		{
			res = list[i];
		}
	}
	return res.hardwareAddress();
}

QString ClientSysInfo::CalculateIPAddress()
{
	QNetworkInterface mac_interface = QNetworkInterface::interfaceFromName(getMacAddress());
	QList<QHostAddress> list_ip = mac_interface.allAddresses();
	QString res;
	for (int i = 0; i < list_ip.size(); i++)
	{
		if (!list_ip[i].isLinkLocal() && !list_ip[i].isLoopback()) {
			res = list_ip[i].toString();
		}
	}
	return res;
}



string ClientSysInfo::CalculateHardDisk_MediaType(string LogicalDisk) { // "C:/" is input paramenter 
	// wstring  str1 = L"\\\\.\\C:"; parameter must be like this 
	LogicalDisk.insert(0, std::string("\\\\.\\"));
	LogicalDisk.erase(LogicalDisk.end() - 1);
	qDebug() << "LogicalDisk driver" << QString(LogicalDisk.c_str());
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

typedef BOOL(WINAPI *LPFN_GLPI)(
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
	PDWORD);

int ClientSysInfo::CalculateCPU_cores() {
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
		_tprintf(TEXT("\nGetLogicalProcessorInformation is not supported.\n"));
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
					_tprintf(TEXT("\nError: Allocation failure\n"));
					return (2);
				}
			}
			else
			{
				_tprintf(TEXT("\nError %d\n"), GetLastError());
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
			_tprintf(TEXT("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}
	return processorCoreCount;
}


int ClientSysInfo::Calculatefree_space(std::string logic_drive) {
	fs::space_info tmpi = fs::space(logic_drive);
	int size_in_GB = tmpi.free / 1024 / 1024 / 1024;
	return size_in_GB;
}

int ClientSysInfo::Calculatecapacity(std::string logic_drive) {
	fs::space_info tmpi = fs::space(logic_drive);
	int size_in_GB = tmpi.capacity / 1024 / 1024 / 1024;
	return size_in_GB;
}
