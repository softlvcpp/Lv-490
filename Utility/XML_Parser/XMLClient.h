#pragma once
#include"pch.h"
#include"Defines.h"
//#include"../../Client/ClientSysInfo.h"

class IXMLClient
{
public:
   // virtual void WriteSystemInformation(string& xml_str, ClientSysInfo& obj) const noexcept = 0;//write client's information in external string
    virtual ~IXMLClient() {};
};

class XMLPARSER_API XMLClient final :public IXMLClient
{
public:
  //  void WriteSystemInformation(string& xml_str, ClientSysInfo& obj) const noexcept override;//write client's information in external string

    //Getters
    string  get_macaddress()const { return m_mac_address; }
    string  get_ipaddress()const { return m_ip_address; }

    int  get_totalram()const { return m_total_ram; }
    int  get_cpunumbers()const { return m_cpu_numbers; }
    int  get_cpuspeed()const { return m_cpu_speed; }

    string  get_cpuvendor()const { return m_cpu_vendor; }

    vector<string>   get_harddisk_type_list()const { return m_harddisk_type_list; }
    vector<int>      get_harddisk_totalsize()const { return m_harddisk_totalsize; }
    vector<int>      get_harddisk_used()const { return m_harddisk_used; }
    vector<int>      get_harddisk_free()const { return m_harddisk_free; }

    //Setters
    void set_macaddress(string other) { m_mac_address = other; }
    void set_ipaddress(string other) { m_ip_address = other; }
    void set_totalram(int other) { m_total_ram = other; }
    void set_cpunumbers(int other) { m_cpu_numbers = other; }
    void set_cpuspeed(int other) { m_cpu_speed = other; }
    void set_cpuvendor(string other) { m_cpu_vendor = other; }
    void set_harddisk_type_list(vector<string> other) { m_harddisk_type_list = other; }
    void set_harddisk_totalsize(vector<int> other) { m_harddisk_totalsize = other; }
    void set_harddisk_used(vector<int> other) { m_harddisk_used = other; }
    void set_harddisk_free(vector<int> other) { m_harddisk_free = other; }

private:
    //clients' information members
    string m_mac_address{ "" };
    string m_ip_address{ "" };

    int m_total_ram{ 0 };
    int m_cpu_numbers{ 0 };
    int m_cpu_speed{ 0 };

    string m_cpu_vendor{ "" };

    vector<string>  m_harddisk_type_list;
    vector<int>     m_harddisk_totalsize;
    vector<int>     m_harddisk_used;
    vector<int>     m_harddisk_free;
};

extern XMLPARSER_API int nXMLParser;