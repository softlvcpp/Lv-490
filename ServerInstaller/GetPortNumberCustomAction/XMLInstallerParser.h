#pragma once
#include "tinyxml2.h"
#include "stdafx.h"

class XMLInstallerParser
{
    std::string m_serverdisplayname = "";
    std::string m_servername = "";
    std::string m_listenport = "";
    std::string m_ipadress = "";
    std::string m_blocking = "";
    std::string m_socket_timeout = "";
    std::string m_filename = "";
    std::string m_loglevel = "";
    std::string m_flush = "";
    std::string m_period_time = "";
    std::string m_maxworkingthreads = "";
public:
	void set_serverdisplayname(std::string_view other) { m_serverdisplayname = other; }
    void set_servername(std::string_view other) { m_servername = other; }
    void set_listenport(std::string_view other) { m_listenport = other; }
    void set_ipadress(std::string_view other) { m_listenport = other; }
    void set_blocking(std::string_view other) { m_blocking = other; }
    void set_socket_timeout(std::string_view other) { m_socket_timeout = other; }
    void set_filename(std::string_view other) { m_filename = other; }
    void set_loglevel(std::string_view other) { m_loglevel = other; }
    void set_flush(std::string_view other) { m_flush = other; }
    void set_period_time(std::string_view other) { m_period_time = other; }
    void set_maxworkingthreads(std::string_view other) { m_maxworkingthreads = other; }

    bool WriteConfig(const std::string& file_path);
};