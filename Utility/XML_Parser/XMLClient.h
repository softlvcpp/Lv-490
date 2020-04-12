#pragma once
#include"pch.h"
#include"Defines.h"

class IXMLClient
{
public:
    virtual void WriteSystemInformation(string& xml_str) const noexcept = 0;//write client's information in external string
    virtual ~IXMLClient() {};
};

class XMLPARSER_API XMLClient final :public IXMLClient
{
public:
   void WriteSystemInformation(string& xml_str) const noexcept override;//write client's information in external string

    //Getters
    string  get_mac_address()const { return m_mac_address; }
    string  get_ip_address()const { return m_ip_address; }

    int  get_total_ram()const { return m_total_ram; }
    int  get_cpu_numbers()const { return m_cpu_numbers; }
    int  get_cpu_speed()const { return m_cpu_speed; }

    string  get_cpu_vendor()const { return m_cpu_vendor; }

    vector<string>   get_hard_disk_type_list()const { return m_hard_disk_type_list; }
    vector<int>      get_hard_disk_totalsize()const { return m_hard_disk_totalsize; }
    vector<int>      get_hard_disk_used()const { return m_hard_disk_used; }
    vector<int>      get_hard_disk_free()const { return m_hard_disk_free; }

   

    //Setters
    void set_mac_address(string other) { m_mac_address = other; }
    void set_ip_address(string other) { m_ip_address = other; }
    void set_total_ram(int other) { m_total_ram = other; }
    void set_cpu_numbers(int other) { m_cpu_numbers = other; }
    void set_cpu_speed(int other) { m_cpu_speed = other; }
    void set_cpu_vendor(string other) { m_cpu_vendor = other; }
    void set_hard_disk_type_list(vector<string> other) { m_hard_disk_type_list = other; }
    void set_hard_disk_totalsize(vector<int> other) { m_hard_disk_totalsize = other; }
    void set_hard_disk_used(vector<int> other) { m_hard_disk_used = other; }
    void set_hard_disk_free(vector<int> other) { m_hard_disk_free = other; }

private:
    //clients' information members
    string m_mac_address{ "" };
    string m_ip_address{ "" };

    int m_total_ram{ 0 };
    int m_cpu_numbers{ 0 };
    int m_cpu_speed{ 0 };

    string m_cpu_vendor{ "" };

    vector<string>  m_hard_disk_type_list;
    vector<int>     m_hard_disk_totalsize;
    vector<int>     m_hard_disk_used;
    vector<int>     m_hard_disk_free;

    map <int, string> m_processes;
};

extern XMLPARSER_API int nXMLParser;