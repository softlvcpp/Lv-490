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
#include<vector>//ClientSysInfo
#include"tinyxml2.h"
using namespace std;
using namespace filesystem;
using namespace tinyxml2;
// This class is exported from the dll
class XMLPARSER_API CXMLParser {
public:

    class ClientSysInfo
    {
        string OS;
        string MacAddress{ "B4-2E-99-10-AA-2C" };
        string IPAddress{ "127.0.0.1" };
        int TotalRAM{ 100 };
        int CPUNumbers{ 4 };
        string CPUVendor{ "Intel" };
        int CPUSpeed{ 100000 };
        vector<string> HardDisk_type_list{ {"C"},{"D"} };
        vector<int> HardDisk_TotalSize{ 1000,1000 };
        vector<int> HardDisk_Used{ 5000,5000 };
        vector<int> HardDisk_Free{ 1500,1500 };
    public:
        string Get_OS() { return OS; }
        string Get_MacAddress() { return MacAddress; }
        string Get_IPAddress() { return IPAddress; }
        int Get_TotalRAM() { return TotalRAM; }
        int Get_CPUNumbers() { return CPUNumbers; }
        string  Get_CPUVendor() { return CPUVendor; }
        int  Get_CPUSpeed() { return CPUSpeed; }
        vector<string>  Get_HardDisk_type_list() { return HardDisk_type_list; }
        vector<int>  Get_HardDisk_TotalSize() { return HardDisk_TotalSize; }
        vector<int>  Get_HardDisk_Used() { return HardDisk_Used; }
        vector<int>  Get_HardDisk_Free() { return HardDisk_Free; }
    };

    struct outDocument
    {
        string serverdisplayname = "";
        string servername = "";
        string listenport = "";
        string ipadress = "";
        string blocking = "";
        string socket_timeout = "";
        string filename = "";
        string LogLevel = "";
        string flush = "";
        string Period_time = "";
        string maxworkingthreads = "";
    };

    class XML_Parser
    {
    public:
        XML_Parser() = default;
        XMLPARSER_API void Read(const string& file_path);
        XMLPARSER_API void Write(const string& file_path, ClientSysInfo& obj) const;

        XMLPARSER_API outDocument GetData() const noexcept;
    private:
        outDocument out_doc;

    };

    class crypto
    {
        bool encrypt();
        bool decrypt();
    };
	CXMLParser(void);
	// TODO: add your methods here.
};

extern XMLPARSER_API int nXMLParser;

XMLPARSER_API int fnXMLParser(void);
#endif