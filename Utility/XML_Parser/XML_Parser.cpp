// XML_Parser.cpp : Defines the exported functions for the DLL.
//
#pragma once
#include "pch.h"
#include "framework.h"
#include "XML_Parser.h"
#include"tinyxml2.h"

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
#define CONFIG_LOGGIN "LOGGIN"
#define CONFIG_FILENAME "filename"
#define CONFIG_LOGLEVEL "LogLevel"
#define CONFIG_FLUSH "flush"
#define CONFIG_TIME "time"
#define CONFIG_PERIOD_TIME "Period_time"
#define CONFIG_THREADPOOL "threadpool"
#define CONFIG_MAXWORKINGTHREAD "maxworkingthreads"


bool CXMLParser::XMLParser::ReadConfigs(const string& file_path) noexcept
{
    if (!is_regular_file(file_path)) //logger return throw exception("wrong file path");
        return false;

    XMLDocument doc;

    if (doc.LoadFile(file_path.c_str()) != 0) //logger return throw exception("can't load xml file");
        return false;
    XMLElement* pRoot;//smart ptr

    pRoot = doc.FirstChildElement(CONFIG_ROOT);
    if (pRoot == nullptr)
        return false;//file is empty or another format

    /*All checks below forced to avoid abort()*/

    //<Server>
    XMLElement* Server = pRoot->FirstChildElement(CONFIG_SERVER);
    if (Server == nullptr) return false;//no sense to continue;

    if (Server->FirstChildElement(CONFIG_SERVERNAME) != nullptr)
        out_doc.servername = Server->FirstChildElement(CONFIG_SERVERNAME)->GetText();

    if (Server->FirstChildElement(CONFIG_SERVERDISPLAYNAME) != nullptr)
        out_doc.serverdisplayname = Server->FirstChildElement(CONFIG_SERVERDISPLAYNAME)->GetText();

    if (Server->FirstChildElement(CONFIG_LISTENPORT) != nullptr)
        out_doc.listenport = Server->FirstChildElement(CONFIG_LISTENPORT)->GetText();

    if (Server->FirstChildElement(CONFIG_IPADRESS) != nullptr)
        out_doc.ipadress = Server->FirstChildElement(CONFIG_IPADRESS)->GetText();

    //<communicationsetings>
    XMLElement* Settings = pRoot->FirstChildElement(CONFIG_COMMUNICATIONSETTINGS);
    if (Settings == nullptr) return false;//no sense to continue;

    if (Settings->FirstChildElement(CONFIG_BLOCKING) != nullptr)
        out_doc.blocking = Settings->FirstChildElement(CONFIG_BLOCKING)->GetText();

    if (Settings->FirstChildElement(CONFIG_SOCKET_TIMEOUT) != nullptr)
        out_doc.socket_timeout = Settings->FirstChildElement(CONFIG_SOCKET_TIMEOUT)->GetText();

    //<loggin>
    XMLElement* Loggin = pRoot->FirstChildElement(CONFIG_LOGGIN);
    if (Loggin == nullptr) return false;//no sense to continue

    if (Loggin->FirstChildElement(CONFIG_FILENAME) != nullptr)
        out_doc.filename = Loggin->FirstChildElement(CONFIG_FILENAME)->GetText();

    if (Loggin->FirstChildElement(CONFIG_LOGLEVEL) != nullptr)
        out_doc.loglevel = Loggin->FirstChildElement(CONFIG_LOGLEVEL)->GetText();

    if (Loggin->FirstChildElement(CONFIG_FLUSH))
        out_doc.flush = Loggin->FirstChildElement(CONFIG_FLUSH)->GetText();

    //<time>
    if (pRoot->FirstChildElement(CONFIG_TIME)->FirstChildElement(CONFIG_PERIOD_TIME) != nullptr)
        out_doc.period_time = pRoot->FirstChildElement(CONFIG_TIME)->FirstChildElement(CONFIG_PERIOD_TIME)->GetText();

    //<treadpool>
    if (pRoot->FirstChildElement(CONFIG_THREADPOOL)->FirstChildElement(CONFIG_MAXWORKINGTHREAD))
        out_doc.maxworkingthreads = pRoot->FirstChildElement(CONFIG_THREADPOOL)->FirstChildElement(CONFIG_MAXWORKINGTHREAD)->GetText();

    return true;//all is good
}


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

void CXMLParser::XMLParser::WriteSystemInformation(string& xml_str, ClientSysInfo& obj) const noexcept
{
    XMLDocument xmlDoc;
    //root
    XMLNode* pRoot = xmlDoc.NewElement(CLIENT_ROOT);

    xmlDoc.InsertFirstChild(pRoot);
    //client
    XMLElement* pClient = xmlDoc.NewElement(CLIENT_CLIENT);//root->client
    //MAC
    XMLElement* pMAC = xmlDoc.NewElement(CLIENT_MACADRESS);//root->client->MacAddress

    pMAC->SetText(obj.getMacAddress.c_str());
    pClient->InsertFirstChild(pMAC);
    //IP
    XMLElement* pIPAddress = xmlDoc.NewElement(CLIENT_IPADRESS);//root->client->IPAddress
    pIPAddress->SetText(obj.get_IPAddress().c_str());
    pClient->InsertFirstChild(pIPAddress);

    //SystemInfo
    XMLElement* pSystemInformation = xmlDoc.NewElement(CLIENT_SYSTEMINFORMATION);//root->client->SystemInformation

    vector<string> HardDisk_type_list = obj.get_HardDisk_type_list();
    vector<int> HardDisk_TotalSize = obj.get_HardDisk_TotalSize();
    vector<int> HardDisk_Used = obj.get_HardDisk_Used();
    vector<int> HardDisk_Free = obj.get_HardDisk_Free();

    //HardDisk loop
    XMLElement* pHardDisk;
    for (size_t i = 0; i < HardDisk_type_list.size(); ++i)
    {
        pHardDisk = xmlDoc.NewElement(CLIENT_HARDDISK);//root->client->SystemInformation->HardDisk

        pHardDisk->SetAttribute(CLIENT_TYPE, "HDD");//as default, to be continue......

        pHardDisk->SetAttribute(CLIENT_DRIVE, HardDisk_type_list[i].c_str());

        XMLElement* pHardDisk_TotalSize = xmlDoc.NewElement(CLIENT_HARDDISK_TOTALSIZE);//root->client->SystemInformation->HardDisk
        pHardDisk_TotalSize->SetText(HardDisk_TotalSize[i]);
        XMLElement* pHardDisk_Used = xmlDoc.NewElement(CLIENT_HARDDISK_USED);//root->client->SystemInformation->HardDisk
        pHardDisk_Used->SetText(HardDisk_Used[i]);
        XMLElement* pHardDisk_Free = xmlDoc.NewElement(CLIENT_HARDDISK_FREE);//root->client->SystemInformation->HardDisk
        pHardDisk_Free->SetText(HardDisk_Free[i]);

        pHardDisk->InsertEndChild(pHardDisk_TotalSize);
        pHardDisk->InsertEndChild(pHardDisk_Used);
        pHardDisk->InsertEndChild(pHardDisk_Free);

        pSystemInformation->InsertEndChild(pHardDisk);
    }
    //RAM
    XMLElement* pTotalRAM = xmlDoc.NewElement(CLIENT_TOTALRAM);//root->client->SystemInformation->TotalRAM
    pTotalRAM->SetText(obj.get_TotalRAM());
    pSystemInformation->InsertEndChild(pTotalRAM);
    //CPU
    XMLElement* CPU = xmlDoc.NewElement(CLIENT_CPU);//root->client->SystemInformation->CPU

    XMLElement* CPUNumbers = xmlDoc.NewElement(CLIENT_CPU_NUMBERS);//root->client->SystemInformation->CPU
    CPUNumbers->SetText(obj.get_CPUNumbers());
    CPU->InsertEndChild(CPUNumbers);
    XMLElement* CPUVendor = xmlDoc.NewElement(CLIENT_CPU_VENDOR);//root->client->SystemInformation->CPU
    CPUVendor->SetText(obj.get_CPUVendor().c_str());
    CPU->InsertEndChild(CPUVendor);
    XMLElement* CPUSpeed = xmlDoc.NewElement(CLIENT_CPU_SPEED);//root->client->SystemInformation->CPU
    CPUSpeed->SetText(obj.get_CPUSpeed());
    CPU->InsertEndChild(CPUSpeed);

    //end
    pSystemInformation->InsertEndChild(CPU);
    pClient->InsertEndChild(pSystemInformation);
    pRoot->InsertFirstChild(pClient);


    XMLPrinter printer;
    xmlDoc.Accept(&printer);
    xml_str = printer.CStr();
}

bool CXMLParser::XMLParser::PrepareToDBManager(string& xml_str)noexcept
{
    if (xml_str == "")//logger
        return false;

    XMLDocument doc;

    if (doc.Parse(xml_str.c_str()) != 0) //logger return throw exception("can't load xml file");
        return false;

    //<root>
    XMLElement* pRoot;//smart ptr

    pRoot = doc.FirstChildElement(CLIENT_ROOT);
    if (pRoot == nullptr)
        return false;//file is empty or another format

    //<Client>
    XMLElement* pClient = pRoot->FirstChildElement(CLIENT_CLIENT);
    if (pClient == nullptr) return false;//no sense to continue;

    if (pClient->FirstChildElement(CLIENT_IPADRESS) != nullptr)
        ip_address = pClient->FirstChildElement(CLIENT_IPADRESS)->GetText();//ip_address

    if (pClient->FirstChildElement(CLIENT_MACADRESS) != nullptr)
        mac_address = pClient->FirstChildElement(CLIENT_MACADRESS)->GetText();//mac_address

    //<SystemInformation>
    XMLElement* pSysInfo = pClient->FirstChildElement(CLIENT_SYSTEMINFORMATION);
    if (pSysInfo == nullptr) return false;//no sense to continue;
    //<HardDisk> loop
    XMLElement* pHardDisk = pSysInfo->FirstChildElement(CLIENT_HARDDISK);
    if (pHardDisk == nullptr) return false;//no sense to continue;

    for (XMLElement* e = pSysInfo->FirstChildElement(CLIENT_HARDDISK); e != nullptr; e = e->NextSiblingElement(CLIENT_HARDDISK))
    {
        harddisk_type_list.emplace_back(e->Attribute(CLIENT_DRIVE));
        harddisk_totalsize.emplace_back(e->FirstChildElement(CLIENT_HARDDISK_TOTALSIZE)->IntText());
        harddisk_used.emplace_back(e->FirstChildElement(CLIENT_HARDDISK_USED)->IntText());
        harddisk_free.emplace_back(e->FirstChildElement(CLIENT_HARDDISK_FREE)->IntText());
    }

    //<totalRAM>
    if (pSysInfo->FirstChildElement(CLIENT_TOTALRAM) != nullptr)
        total_ram = pSysInfo->FirstChildElement(CLIENT_TOTALRAM)->IntText();

    //CPU
    XMLElement* pCPU = pSysInfo->FirstChildElement(CLIENT_CPU);
    if (pCPU == nullptr) return false;//no sense to continue;

    if (pCPU->FirstChildElement(CLIENT_CPU_NUMBERS) != nullptr)
        cpu_numbers = pCPU->FirstChildElement(CLIENT_CPU_NUMBERS)->IntText();

    if (pCPU->FirstChildElement(CLIENT_CPU_VENDOR) != nullptr)
        cpu_vendor = pCPU->FirstChildElement(CLIENT_CPU_VENDOR)->GetText();

    if (pCPU->FirstChildElement(CLIENT_CPU_SPEED))
        cpu_speed = pCPU->FirstChildElement(CLIENT_CPU_SPEED)->IntText();

    return true;//all is good
}



// This is an example of an exported variable
XMLPARSER_API int nXMLParser=0;

// This is an example of an exported function.
XMLPARSER_API int fnXMLParser(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CXMLParser::CXMLParser()
{
    return;
}
