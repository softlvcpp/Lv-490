#include "stdafx.h"
#include "tinyxml2.h"
#include "../../Utility/XML_Parser/Defines.h"
#include "XMLInstallerParser.h"
bool XMLInstallerParser::WriteConfig(const std::string& file_path)
{
    using namespace tinyxml2;
    XMLDocument xml_doc;
    //root
    XMLNode* root = xml_doc.NewElement(CONFIG_ROOT);

    xml_doc.InsertFirstChild(root);
    //server
    XMLElement* server = xml_doc.NewElement(CONFIG_SERVER);//root->server

    XMLElement* servername = xml_doc.NewElement(CONFIG_SERVERNAME);//root->server->servername
    servername->SetText(this->m_servername.c_str());
    server->InsertEndChild(servername);
    root->InsertEndChild(server);

    XMLElement* server_display_name = xml_doc.NewElement(CONFIG_SERVERDISPLAYNAME);//root->server->serverdisplayname
    server_display_name->SetText(m_serverdisplayname.c_str());
    server->InsertEndChild(server_display_name);

    XMLElement* listenport = xml_doc.NewElement(CONFIG_LISTENPORT);//root->server->listenport
    listenport->SetText(m_listenport.c_str());
    server->InsertEndChild(listenport);
    //communicationseting
    XMLElement* communication = xml_doc.NewElement(CONFIG_COMMUNICATIONSETTINGS);//root->communicationsetings

    XMLElement* blocking = xml_doc.NewElement(CONFIG_BLOCKING);//root->communicationsetings->blocking
    blocking->SetText(m_blocking.c_str());
    communication->InsertEndChild(blocking);

    XMLElement* socket_timeout = xml_doc.NewElement(CONFIG_SOCKET_TIMEOUT);//root->communicationsetings->socket_timeout
    socket_timeout->SetText(m_socket_timeout.c_str());
    communication->InsertEndChild(socket_timeout);
    root->InsertEndChild(communication);

    //loggin
    XMLElement* loggin = xml_doc.NewElement(CONFIG_LOGGIN);//root->loggin

    XMLElement* filename = xml_doc.NewElement(CONFIG_FILENAME);//root->loggin->filename
    filename->SetText(m_filename.c_str());
    loggin->InsertEndChild(filename);

    XMLElement* loglevel = xml_doc.NewElement(CONFIG_LOGLEVEL);//root->loggin->LogLevel
    loglevel->SetText(m_loglevel.c_str());
    loggin->InsertEndChild(loglevel);

    XMLElement* flush = xml_doc.NewElement(CONFIG_FLUSH);//root->loggin->flush
    flush->SetText(m_flush.c_str());
    loggin->InsertEndChild(flush);
    root->InsertEndChild(loggin);

    XMLElement* time = xml_doc.NewElement(CONFIG_TIME);//root->time

    XMLElement* period_time = xml_doc.NewElement(CONFIG_PERIOD_TIME);//root->time->Period_time
    period_time->SetText(m_period_time.c_str());
    time->InsertEndChild(period_time);
    root->InsertEndChild(time);

    XMLElement* threadpool = xml_doc.NewElement(CONFIG_THREADPOOL);//root->threadpool

    XMLElement* max_working_threads = xml_doc.NewElement(CONFIG_MAXWORKINGTHREAD);//root->threadpool->maxworkingthreads
    max_working_threads->SetText(m_maxworkingthreads.c_str());
    threadpool->InsertEndChild(max_working_threads);
    root->InsertEndChild(threadpool);

    std::ofstream ofs(file_path);
    if (ofs.is_open())
    {
        XMLPrinter printer;
        xml_doc.Print(&printer);
        ofs << printer.CStr();
        return true;
    }
    return false;     
    
}
