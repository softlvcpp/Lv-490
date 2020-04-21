#pragma once
#include"pch.h"
#include"Consts.h"
#include <map>

using namespace std;

class XMLPARSER_API IXMLServer
{
public:
    virtual bool ReadConfigs(const string& file_path)noexcept = 0;//read configurations' file for server
    virtual bool PrepareToDBManager(string& xml_str)noexcept = 0;//prepare client's external string to write in DB

    virtual bool WriteConfig(const string& file_path)  noexcept = 0;

    virtual ~IXMLServer() {};
};

class XMLPARSER_API XMLServer :public IXMLServer
{
public:
   
    XMLServer() : m_listenport(0), m_blocking(0),m_socket_timeout(0),m_loglevel(0),
                  m_flush(0), m_period_time(0),m_maxworkingthreads(0),m_total_ram(0),
                  m_cpu_numbers(0),m_cpu_speed(0) {};

    XMLServer(const XMLServer&) = delete;
    XMLServer& operator=(const XMLServer&) = delete;

    bool ReadConfigs(const string& file_path) noexcept override;//read configurations' file for server

    bool PrepareToDBManager(string& xml_str)noexcept override;//prepare client's external string to write in DB

    bool WriteConfig(const string& file_path)  noexcept override;//write configurations' file for server

    //Getters for config file
    const string& get_serverdisplayname()const { return m_serverdisplayname; }
    const string& get_servername()const { return m_servername; }  
    const int& get_listenport()const { return m_listenport; }
    const string& get_ipadress()const { return m_ipadress; }
    const int& get_blocking()const { return m_blocking; }
    const int& get_socket_timeout()const { return m_socket_timeout; }
    const string& get_filename()const { return m_filename; }
    const int& get_loglevel()const { return m_loglevel; }
    const int& get_flush()const { return m_flush; }
    const int& get_period_time()const { return m_period_time; }
    const int& get_maxworkingthreads()const { return m_maxworkingthreads; }

    //Setters for config file
    void set_serverdisplayname(const string& other) { m_serverdisplayname = other; }
    void set_servername(const string& other) { m_servername = other; }
    void set_listenport(const int& other) { m_listenport = other; }
    void set_ipadress(const string& other) { m_ipadress = other; }
    void set_blocking(const int& other) { m_blocking = other; }
    void set_socket_timeout(const int& other) { m_socket_timeout = other; }
    void set_filename(const string& other) { m_filename = other; }
    void set_loglevel(const int& other) { m_loglevel = other; }
    void set_flush(const int& other) { m_flush = other; }
    void set_period_time(const int& other) { m_period_time = other; }
    void set_maxworkingthreads(const int& other) { m_maxworkingthreads = other; }

    //Getters for clients' info
    const string& get_os()const { return m_os; }
    const string& get_mac_address()const { return m_mac_address; }
    const string& get_ip_address()const { return m_ip_address; }

    const int&  get_total_ram()const { return m_total_ram; }
    const int& get_cpu_numbers()const { return m_cpu_numbers; }
    const int& get_cpu_speed()const { return m_cpu_speed; }

    const string& get_cpu_vendor()const { return m_cpu_vendor; }

    const vector<string>&   get_hard_disk_type_list()const { return m_hard_disk_type_list; }
    const vector<string>& get_hard_disk_media_type()const { return m_hard_disk_media_type; }
    const vector<int> get_hard_disk_total_size()const { return m_hard_disk_total_size; }
    const vector<int> get_hard_disk_used()const { return m_hard_disk_used; }
    const vector<int> get_hard_disk_free()const { return m_hard_disk_free; }

    map <int, string>get_processes()const { return m_processes; }

private:
    //config members
    string m_serverdisplayname;
    string m_servername;
    string m_ipadress;
    string m_filename;
    int m_listenport{0};
    int m_blocking{0};
    int m_socket_timeout{0};
    int m_loglevel{0};
    int m_flush{0};
    int m_period_time{0};
    int m_maxworkingthreads{0};

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

