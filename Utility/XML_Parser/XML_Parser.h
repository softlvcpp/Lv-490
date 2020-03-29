// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the XMLPARSER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// XMLPARSER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef PARSER_DLL
#define PARSER_DLL
#ifdef XMLPARSER_EXPORTS
#define XMLPARSER_API __declspec(dllexport)
#else
#define XMLPARSER_API __declspec(dllimport)
#endif

#include "pch.h"
#include <iostream>
#include<filesystem>//is_regular_file
#include<string>
#include<vector>//ClientInfo
#include"tinyxml2.h"
using namespace std;
using namespace filesystem;
using namespace tinyxml2;
// This class is exported from the dll
class XMLPARSER_API CXMLParser {
public:

    struct ClientInfo
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
    };
    struct OutDocument
    {
        string serverdisplayname = "";
        string servername = "";
        string listenport = "";
        string ipadress = "";
        string blocking = "";
        string socket_timeout = "";
        string filename = "";
        string loglevel = "";
        string flush = "";
        string period_time = "";
        string maxworkingthreads = "";
    };

    class XMLParser
    {
    public:
        XMLParser() = default;     

        XMLPARSER_API bool ReadConfigs(const string& file_path)noexcept;//read configuration' file for server
        XMLPARSER_API void WriteSystemInformation(string& xml_str, ClientInfo& obj) const noexcept;//write client's information in external string
        XMLPARSER_API bool PrepareToDBManager(string& xml_str)noexcept;//prepare client's external string to write in DB

        OutDocument get_data() const noexcept { return out_doc; }//getter configs for server

		string get_macaddress()const noexcept { return mac_address; }//client info
        string get_ipaddress()const noexcept { return ip_address; }//
        int get_totalram()const noexcept { return total_ram; }//
        int get_cpunumbers()const noexcept { return cpu_numbers; }//
        string  get_cpuvendor()const noexcept { return cpu_vendor; }//
        int  get_cpuspeed()const noexcept { return cpu_speed; }//
        vector<string>  get_harddisk_type_list()const noexcept { return harddisk_type_list; }//
        vector<int>  get_harddisk_totalsize() const noexcept { return harddisk_totalsize; }//
        vector<int>  get_harddisk_used()const noexcept { return harddisk_used; }//
        vector<int>  get_harddisk_free()const noexcept { return harddisk_free; }//
    private:
        OutDocument out_doc;//configs for server

        string mac_address{ "" };//client info
        string ip_address{ "" };//
        int total_ram{ 0 };//
        int cpu_numbers{ 0 };//
        string cpu_vendor{ "" };//
        int cpu_speed{ 0 };//
        vector<string> harddisk_type_list;//
        vector<int> harddisk_totalsize;//
        vector<int> harddisk_used;//
        vector<int> harddisk_free;//
    };
	CXMLParser(void);
	// TODO: add your methods here.
};

extern XMLPARSER_API int nXMLParser;

XMLPARSER_API int fnXMLParser(void);
#endif