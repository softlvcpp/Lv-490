#pragma once
#include "pch.h"
#include"XMLServer.h"


bool XMLServer::ReadConfigs(const string& file_path) noexcept
{
    if (!is_regular_file(file_path)) //logger return throw exception("wrong file path");
        return false;

    XMLDocument doc;

    if (doc.LoadFile(file_path.c_str()) != 0) //logger return throw exception("can't load xml file");
        return false;
    XMLElement* root;

    root = doc.FirstChildElement(g_CONFIG_ROOT);
    if (root == nullptr)
        return false;//file is empty or another format

    /*All checks below forced to avoid abort()*/

    //<Server>
    XMLElement* server = root->FirstChildElement(g_CONFIG_SERVER);
    if (server == nullptr) return false;//no sense to continue;

    if (server->FirstChildElement(g_CONFIG_SERVERNAME) != nullptr)
        m_servername = server->FirstChildElement(g_CONFIG_SERVERNAME)->GetText();

    if (server->FirstChildElement(g_CONFIG_SERVERDISPLAYNAME) != nullptr)
        m_serverdisplayname = server->FirstChildElement(g_CONFIG_SERVERDISPLAYNAME)->GetText();

    if (server->FirstChildElement(g_CONFIG_LISTENPORT) != nullptr)
        m_listenport = server->FirstChildElement(g_CONFIG_LISTENPORT)->IntText();

    if (server->FirstChildElement(g_CONFIG_IPADRESS) != nullptr)
        m_ipadress = server->FirstChildElement(g_CONFIG_IPADRESS)->GetText();

    //<communicationsetings>
    XMLElement* settings = root->FirstChildElement(g_CONFIG_COMMUNICATIONSETTINGS);
    if (settings == nullptr) return false;//no sense to continue;

    if (settings->FirstChildElement(g_CONFIG_BLOCKING) != nullptr)
        m_blocking = settings->FirstChildElement(g_CONFIG_BLOCKING)->IntText();

    if (settings->FirstChildElement(g_CONFIG_SOCKET_TIMEOUT) != nullptr)
        m_socket_timeout = settings->FirstChildElement(g_CONFIG_SOCKET_TIMEOUT)->IntText();

    //<loggin>
    XMLElement* loggin = root->FirstChildElement(g_CONFIG_LOGGIN);
    if (loggin == nullptr) return false;//no sense to continue

    if (loggin->FirstChildElement(g_CONFIG_FILENAME) != nullptr)
        m_filename = loggin->FirstChildElement(g_CONFIG_FILENAME)->GetText();

    if (loggin->FirstChildElement(g_CONFIG_LOGLEVEL) != nullptr)
        m_loglevel = loggin->FirstChildElement(g_CONFIG_LOGLEVEL)->IntText();

    if (loggin->FirstChildElement(g_CONFIG_FLUSH))
        m_flush = loggin->FirstChildElement(g_CONFIG_FLUSH)->IntText();

    //<time>
    if (root->FirstChildElement(g_CONFIG_TIME)->FirstChildElement(g_CONFIG_PERIOD_TIME) != nullptr)
        m_period_time = root->FirstChildElement(g_CONFIG_TIME)->FirstChildElement(g_CONFIG_PERIOD_TIME)->IntText();

    //<treadpool>
    if (root->FirstChildElement(g_CONFIG_THREADPOOL)->FirstChildElement(g_CONFIG_MAXWORKINGTHREAD))
        m_maxworkingthreads = root->FirstChildElement(g_CONFIG_THREADPOOL)->FirstChildElement(g_CONFIG_MAXWORKINGTHREAD)->IntText();

    return true;//all is good
}


bool XMLServer::WriteConfig(const string& file_path) noexcept
{
    XMLDocument xml_doc;
    //root
    XMLNode* root = xml_doc.NewElement(g_CONFIG_ROOT);

    xml_doc.InsertFirstChild(root);
    //server
    XMLElement* server = xml_doc.NewElement(g_CONFIG_SERVER);//root->server

    XMLElement* servername = xml_doc.NewElement(g_CONFIG_SERVERNAME);//root->server->servername
    servername->SetText(this->m_servername.c_str());
    server->InsertEndChild(servername);
    root->InsertEndChild(server);

    XMLElement* server_display_name = xml_doc.NewElement(g_CONFIG_SERVERDISPLAYNAME);//root->server->serverdisplayname
    server_display_name->SetText(m_serverdisplayname.c_str());
    server->InsertEndChild(server_display_name);

    XMLElement* listenport = xml_doc.NewElement(g_CONFIG_LISTENPORT);//root->server->listenport
    listenport->SetText(m_listenport);
    server->InsertEndChild(listenport);
    //communicationseting
    XMLElement* communication = xml_doc.NewElement(g_CONFIG_COMMUNICATIONSETTINGS);//root->communicationsetings

    XMLElement* blocking = xml_doc.NewElement(g_CONFIG_BLOCKING);//root->communicationsetings->blocking
    blocking->SetText(m_blocking);
    communication->InsertEndChild(blocking);

    XMLElement* socket_timeout = xml_doc.NewElement(g_CONFIG_SOCKET_TIMEOUT);//root->communicationsetings->socket_timeout
    socket_timeout->SetText(m_socket_timeout);
    communication->InsertEndChild(socket_timeout);
    root->InsertEndChild(communication);

    //loggin
    XMLElement* loggin = xml_doc.NewElement(g_CONFIG_LOGGIN);//root->loggin

    XMLElement* filename = xml_doc.NewElement(g_CONFIG_FILENAME);//root->loggin->filename
    filename->SetText(m_filename.c_str());
    loggin->InsertEndChild(filename);

    XMLElement* loglevel = xml_doc.NewElement(g_CONFIG_LOGLEVEL);//root->loggin->LogLevel
    loglevel->SetText(m_loglevel);
    loggin->InsertEndChild(loglevel);

    XMLElement* flush = xml_doc.NewElement(g_CONFIG_FLUSH);//root->loggin->flush
    flush->SetText(m_flush);
    loggin->InsertEndChild(flush);
    root->InsertEndChild(loggin);

    XMLElement* time = xml_doc.NewElement(g_CONFIG_TIME);//root->time

    XMLElement* period_time = xml_doc.NewElement(g_CONFIG_PERIOD_TIME);//root->time->Period_time
    period_time->SetText(m_period_time);
    time->InsertEndChild(period_time);
    root->InsertEndChild(time);

    XMLElement* threadpool = xml_doc.NewElement(g_CONFIG_THREADPOOL);//root->threadpool

    XMLElement* max_working_threads = xml_doc.NewElement(g_CONFIG_MAXWORKINGTHREAD);//root->threadpool->maxworkingthreads
    max_working_threads->SetText(m_maxworkingthreads);
    threadpool->InsertEndChild(max_working_threads);
    root->InsertEndChild(threadpool);

    return !xml_doc.SaveFile(file_path.c_str());
}

bool XMLServer::PrepareToDBManager(string& xml_str)noexcept
{
    if (xml_str == "")
        return false;

    XMLDocument doc;

    if (doc.Parse(xml_str.c_str()) != 0)
        return false;

    //<root>
    XMLElement* root;

    root = doc.FirstChildElement(g_CLIENT_ROOT);
    if (root == nullptr)
        return false;

    //<Client>
    XMLElement* client = root->FirstChildElement(g_CLIENT_CLIENT);
    if (client == nullptr) return false;//no sense to continue;

    if (client->FirstChildElement(g_CLIENT_OPERATION_SYSTEM) != nullptr)
        if (client->FirstChildElement(g_CLIENT_OPERATION_SYSTEM)->GetText())
            m_os = client->FirstChildElement(g_CLIENT_OPERATION_SYSTEM)->GetText();

    if (client->FirstChildElement(g_CLIENT_IPADRESS) != nullptr)
        if (client->FirstChildElement(g_CLIENT_IPADRESS)->GetText())
            m_ip_address = client->FirstChildElement(g_CLIENT_IPADRESS)->GetText();

    if (client->FirstChildElement(g_CLIENT_MACADRESS) != nullptr)
        if (client->FirstChildElement(g_CLIENT_MACADRESS)->GetText() != nullptr)
            m_mac_address = client->FirstChildElement(g_CLIENT_MACADRESS)->GetText();

    //<SystemInformation>
    XMLElement* sys_info = client->FirstChildElement(g_CLIENT_SYSTEMINFORMATION);
    if (sys_info == nullptr) return false;//no sense to continue;
    //<HardDisk> loop
    XMLElement* hard_disk = sys_info->FirstChildElement(g_CLIENT_HARDDISK);
    if (hard_disk == nullptr) return false;//no sense to continue;

    for (XMLElement* e = sys_info->FirstChildElement(g_CLIENT_HARDDISK); e != nullptr; e = e->NextSiblingElement(g_CLIENT_HARDDISK))
    {
        m_hard_disk_type_list.emplace_back(e->Attribute(g_CLIENT_DRIVE));
        m_hard_disk_media_type.emplace_back(e->Attribute(g_CLIENT_TYPE));
        m_hard_disk_total_size.emplace_back(e->FirstChildElement(g_CLIENT_HARDDISK_TOTALSIZE)->IntText());
        m_hard_disk_used.emplace_back(e->FirstChildElement(g_CLIENT_HARDDISK_USED)->IntText());
        m_hard_disk_free.emplace_back(e->FirstChildElement(g_CLIENT_HARDDISK_FREE)->IntText());
    }

    //<totalRAM>
    if (sys_info->FirstChildElement(g_CLIENT_TOTALRAM) != nullptr)
        if (sys_info->FirstChildElement(g_CLIENT_TOTALRAM)->IntText())
            m_total_ram = sys_info->FirstChildElement(g_CLIENT_TOTALRAM)->IntText();

    //CPU
    XMLElement* cpu_branch = sys_info->FirstChildElement(g_CLIENT_CPU);
    if (cpu_branch == nullptr) return false;//no sense to continue;

    if (cpu_branch->FirstChildElement(g_CLIENT_CPU_NUMBERS) != nullptr)
        if (cpu_branch->FirstChildElement(g_CLIENT_CPU_NUMBERS)->IntText())
            m_cpu_numbers = cpu_branch->FirstChildElement(g_CLIENT_CPU_NUMBERS)->IntText();

    if (cpu_branch->FirstChildElement(g_CLIENT_CPU_VENDOR) != nullptr)
        if (cpu_branch->FirstChildElement(g_CLIENT_CPU_VENDOR)->GetText())
    m_cpu_vendor = cpu_branch->FirstChildElement(g_CLIENT_CPU_VENDOR)->GetText();

    if (cpu_branch->FirstChildElement(g_CLIENT_CPU_SPEED))
        if (cpu_branch->FirstChildElement(g_CLIENT_CPU_SPEED)->IntText())
            m_cpu_speed = cpu_branch->FirstChildElement(g_CLIENT_CPU_SPEED)->IntText();

    //ProcessInformation
    XMLElement* process_info = sys_info->FirstChildElement(g_CLIENT_PROCESSINFORMATION);
    if (process_info == nullptr) return false;//no sense to continue;

    for (XMLElement* e = sys_info->FirstChildElement(g_CLIENT_PROCESSINFORMATION); e != nullptr; e = e->NextSiblingElement(g_CLIENT_PROCESSINFORMATION))
        m_processes.emplace(stoi(e->Attribute(g_CLIENT_PROCESS_ATTRIBUTE_PID)), string(e->Attribute(g_CLIENT_PROCESS_ATTRIBUTE_NAME)));

    return true;//all is good
}
