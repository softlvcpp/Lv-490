#pragma once
#include"pch.h"
#include"Defines.h"


class IXMLServer
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

    bool WriteConfig(const string& file_path)  noexcept override;
    //Getters
    string get_serverdisplayname()const { return m_serverdisplayname; }
    string get_servername()const { return m_servername; }
    string get_listenport()const { return m_listenport; }
    string get_ipadress()const { return m_listenport; }
    string get_blocking()const { return m_blocking; }
    string get_socket_timeout()const { return m_socket_timeout; }
    string get_filename()const { return m_filename; }
    string get_loglevel()const { return m_loglevel; }
    string get_flush()const { return m_flush; }
    string get_period_time()const { return m_period_time; }
    string get_maxworkingthreads()const { return m_maxworkingthreads; }

    //Setters
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

    //clients' information members
    string  m_mac_address;
    string  m_ip_address;
    int     m_total_ram;
    int     m_cpu_numbers;
    string  m_cpu_vendor;
    int     m_cpu_speed;

    vector<string>  m_harddisk_type_list;
    vector<string>  m_hard_disk_media_type;
    vector<int>     m_harddisk_totalsize;
    vector<int>     m_harddisk_used;
    vector<int>     m_harddisk_free;
    
    map <int, string> m_processes;
};

extern XMLPARSER_API int nXMLParser;