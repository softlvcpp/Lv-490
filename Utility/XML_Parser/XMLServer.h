#pragma once
#include"pch.h"
#include"Defines.h"

using namespace std;

class XMLPARSER_API IXMLServer
{
public:
    virtual bool ReadConfigs(const string& file_path)noexcept = 0;//read configurations' file for server
    virtual bool PrepareToDBManager(string& xml_str)noexcept = 0;//prepare client's external string to write in DB

    virtual bool WriteConfig(const string& file_path)  noexcept = 0;

    virtual ~IXMLServer() {};
};

class XMLPARSER_API XMLServer final :public IXMLServer
{
public:
    bool ReadConfigs(const string& file_path) noexcept override;//read configurations' file for server

    bool PrepareToDBManager(string& xml_str)noexcept override;//prepare client's external string to write in DB

    bool WriteConfig(const string& file_path)  noexcept override;//write configurations' file for server

    //Getters for config file
    string get_serverdisplayname()const { return m_serverdisplayname; }
    string get_servername()const { return m_servername; }
    string get_listenport()const { return m_listenport; }
    string get_ipadress()const { return m_ipadress; }
    string get_blocking()const { return m_blocking; }
    string get_socket_timeout()const { return m_socket_timeout; }
    string get_filename()const { return m_filename; }
    string get_loglevel()const { return m_loglevel; }
    string get_flush()const { return m_flush; }
    string get_period_time()const { return m_period_time; }
    string get_maxworkingthreads()const { return m_maxworkingthreads; }

    //Setters for config file
    void set_serverdisplayname(string other) { m_serverdisplayname = other; }
    void set_servername(string other) { m_servername = other; }
    void set_listenport(string other) { m_listenport = other; }
    void set_ipadress(string other) { m_listenport = other; }
    void set_blocking(string other) { m_blocking = other; }
    void set_socket_timeout(string other) { m_socket_timeout = other; }
    void set_filename(string other) { m_filename = other; }
    void set_loglevel(string other) { m_loglevel = other; }
    void set_flush(string other) { m_flush = other; }
    void set_period_time(string other) { m_period_time = other; }
    void set_maxworkingthreads(string other) { m_maxworkingthreads = other; }

    //Getters for clients' info
    string  get_os()const { return m_os; }
    string  get_mac_address()const { return m_mac_address; }
    string  get_ip_address()const { return m_ip_address; }

    int  get_total_ram()const { return m_total_ram; }
    int  get_cpu_numbers()const { return m_cpu_numbers; }
    int  get_cpu_speed()const { return m_cpu_speed; }

    string  get_cpu_vendor()const { return m_cpu_vendor; }

    vector<string>   get_hard_disk_type_list()const { return m_hard_disk_type_list; }
    vector<string>   get_hard_disk_media_type()const { return m_hard_disk_media_type; }
    vector<int>      get_hard_disk_total_size()const { return m_hard_disk_total_size; }
    vector<int>      get_hard_disk_used()const { return m_hard_disk_used; }
    vector<int>      get_hard_disk_free()const { return m_hard_disk_free; }

    map <int, string>get_processes()const { return m_processes; }

private:
    //config members
    string m_serverdisplayname = "";
    string m_servername = "";
    string m_listenport = "";
    string m_ipadress = "";
    string m_blocking = "";
    string m_socket_timeout = "";
    string m_filename = "";
    string m_loglevel = "";
    string m_flush = "";
    string m_period_time = "";
    string m_maxworkingthreads = "";

private:
    //clients' information members
    string m_os;
    string m_mac_address;
    string m_ip_address;

    int m_total_ram;
    int m_cpu_numbers;
    int m_cpu_speed;

    string m_cpu_vendor;

    vector<string>  m_hard_disk_type_list;
    vector<string>  m_hard_disk_media_type;
    vector<int>     m_hard_disk_total_size;
    vector<int>     m_hard_disk_used;
    vector<int>     m_hard_disk_free;

    map <int, string> m_processes;
};

extern XMLPARSER_API int nXMLParser;