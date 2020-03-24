#pragma once
#include <iostream>
#include<filesystem>//is_regular_file
#include<string>
#include<vector>
#include"tinyxml2.h"
using namespace std;
using namespace filesystem;
using namespace tinyxml2;

struct outDocument
{
    string serverdisplayname = "";
    string servername = "";
    string listenport = "";
    string ipadress = "";
    string blocking = "";
    string socket_timeout = "";
    string filename = "";
    string LogLevel = "";
    string flush = "";
    string Period_time = "";
    string maxworkingthreads = "";
};

class XML_Parser
{
public:
    XML_Parser() = default;
    void Read(const string &file_path);
    void Write(const string &file_path,const ClientSysInfo &obj) const;

    outDocument GetData() const noexcept;

private:
    ClientSysInfo client_info;
    outDocument out_doc;

};