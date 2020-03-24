#pragma once
#include"XML_Parcer.h"

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


void XML_Parser::Read(const string &file_path)
{    
    if (!is_regular_file(file_path)) //logger return throw exception("wrong file path");
        return;

    XMLDocument doc;

    if (doc.LoadFile(file_path.c_str()) != 0) //logger return throw exception("can't load xml file");
        return;
    XMLElement* pRoot;

    pRoot = doc.FirstChildElement(CONFIG_ROOT);
    if (pRoot == nullptr)
        return;//file is empty or another format

    /*All checks below forced to avoid abort()*/

    //<Server>
    XMLElement* Server = pRoot->FirstChildElement(CONFIG_SERVER);
    if (Server == nullptr) return;//no sense to continue;
    
    if(Server->FirstChildElement(CONFIG_SERVERNAME)!=nullptr)
    out_doc.servername = Server->FirstChildElement(CONFIG_SERVERNAME)->GetText();

    if(Server->FirstChildElement(CONFIG_SERVERDISPLAYNAME)!=nullptr)
    out_doc.serverdisplayname = Server->FirstChildElement(CONFIG_SERVERDISPLAYNAME)->GetText();

    if(Server->FirstChildElement(CONFIG_LISTENPORT)!=nullptr)
    out_doc.listenport = Server->FirstChildElement(CONFIG_LISTENPORT)->GetText();

    if(Server->FirstChildElement(CONFIG_IPADRESS)!=nullptr)
    out_doc.ipadress = Server->FirstChildElement(CONFIG_IPADRESS)->GetText();

    //<communicationsetings>
    XMLElement* Settings = pRoot->FirstChildElement(CONFIG_COMMUNICATIONSETTINGS);
    if (Settings == nullptr) return;//no sense to continue;

    if(Settings->FirstChildElement(CONFIG_BLOCKING)!=nullptr)
    out_doc.blocking = Settings->FirstChildElement(CONFIG_BLOCKING)->GetText();

    if(Settings->FirstChildElement(CONFIG_SOCKET_TIMEOUT)!=nullptr)
    out_doc.socket_timeout = Settings->FirstChildElement(CONFIG_SOCKET_TIMEOUT)->GetText();

    //<loggin>
    XMLElement* Loggin = pRoot->FirstChildElement(CONFIG_LOGGIN);
    if (Loggin == nullptr) return;//no sense to continue

    if(Loggin->FirstChildElement(CONFIG_FILENAME)!=nullptr)
    out_doc.filename = Loggin->FirstChildElement(CONFIG_FILENAME)->GetText();

    if(Loggin->FirstChildElement(CONFIG_LOGLEVEL)!=nullptr)
    out_doc.LogLevel = Loggin->FirstChildElement(CONFIG_LOGLEVEL)->GetText();

    if(Loggin->FirstChildElement(CONFIG_FLUSH))
    out_doc.flush = Loggin->FirstChildElement(CONFIG_FLUSH)->GetText();

    //<time>
    if(pRoot->FirstChildElement(CONFIG_TIME)->FirstChildElement(CONFIG_PERIOD_TIME)!=nullptr)
    out_doc.Period_time = pRoot->FirstChildElement(CONFIG_TIME)->FirstChildElement(CONFIG_PERIOD_TIME)->GetText();

    //<treadpool>
    if (pRoot->FirstChildElement(CONFIG_THREADPOOL)->FirstChildElement(CONFIG_MAXWORKINGTHREAD))
    out_doc.maxworkingthreads = pRoot->FirstChildElement(CONFIG_THREADPOOL)->FirstChildElement(CONFIG_MAXWORKINGTHREAD)->GetText();
}

outDocument XML_Parser::GetData() const noexcept
{
    return this->out_doc;
}

void XML_Parser::Write(const string &file_path,const  ClientSysInfo &obj) const
{
       
}

