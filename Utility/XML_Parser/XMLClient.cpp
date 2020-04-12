#pragma once
#include"pch.h"
#include"XMLClient.h"


void XMLClient::WriteSystemInformation(string &xml_str) const noexcept
{
    XMLDocument xml_doc;
    //root
    XMLNode* root = xml_doc.NewElement(CLIENT_ROOT);
    
    xml_doc.InsertFirstChild(root);
    //client
    XMLElement* client = xml_doc.NewElement(CLIENT_CLIENT);//root->client
    //MAC
    XMLElement* mac = xml_doc.NewElement(CLIENT_MACADRESS);//root->client->MacAddress

    mac->SetText(m_mac_address.c_str());
    client->InsertFirstChild(mac);
    //IP
    XMLElement* ip = xml_doc.NewElement(CLIENT_IPADRESS);//root->client->IPAddress
    ip->SetText(m_ip_address.c_str());
    client->InsertFirstChild(ip);

    //SystemInfo
    XMLElement* sys_info = xml_doc.NewElement(CLIENT_SYSTEMINFORMATION);//root->client->SystemInformation
    
    vector<string> HardDisk_type_list= m_hard_disk_type_list;
    vector<int> HardDisk_TotalSize= m_hard_disk_totalsize;
    vector<int> HardDisk_Used= m_hard_disk_used;
    vector<int> HardDisk_Free= m_hard_disk_free;

    //HardDisk loop
    XMLElement* hard_disk;
    for (size_t i=0;i<HardDisk_type_list.size();++i)
    {
        hard_disk = xml_doc.NewElement(CLIENT_HARDDISK);//root->client->SystemInformation->HardDisk

        hard_disk->SetAttribute(CLIENT_TYPE, "HDD");//as default, to be continue......

        hard_disk->SetAttribute(CLIENT_DRIVE, HardDisk_type_list[i].c_str());
        
        XMLElement* pHardDisk_TotalSize = xml_doc.NewElement(CLIENT_HARDDISK_TOTALSIZE);//root->client->SystemInformation->HardDisk
        pHardDisk_TotalSize->SetText(HardDisk_TotalSize[i]);
        XMLElement* pHardDisk_Used = xml_doc.NewElement(CLIENT_HARDDISK_USED);//root->client->SystemInformation->HardDisk
        pHardDisk_Used->SetText(HardDisk_Used[i]);
        XMLElement* pHardDisk_Free = xml_doc.NewElement(CLIENT_HARDDISK_FREE);//root->client->SystemInformation->HardDisk
        pHardDisk_Free->SetText(HardDisk_Free[i]);

        hard_disk->InsertEndChild(pHardDisk_TotalSize);
        hard_disk->InsertEndChild(pHardDisk_Used);
        hard_disk->InsertEndChild(pHardDisk_Free);

        sys_info->InsertEndChild(hard_disk);
    }
    //RAM
    XMLElement* ram = xml_doc.NewElement(CLIENT_TOTALRAM);//root->client->SystemInformation->TotalRAM
    ram->SetText(m_total_ram);
    sys_info->InsertEndChild(ram);
    //CPU
    XMLElement* cpu_branch = xml_doc.NewElement(CLIENT_CPU);//root->client->SystemInformation->CPU

    XMLElement* cpu_numbers = xml_doc.NewElement(CLIENT_CPU_NUMBERS);//root->client->SystemInformation->CPU
    cpu_numbers->SetText(m_cpu_numbers);
    cpu_branch->InsertEndChild(cpu_numbers);

    XMLElement* cpu_vendor = xml_doc.NewElement(CLIENT_CPU_VENDOR);//root->client->SystemInformation->CPU
    cpu_vendor->SetText(m_cpu_vendor.c_str());
    cpu_branch->InsertEndChild(cpu_vendor);

    XMLElement* cpu_speed = xml_doc.NewElement(CLIENT_CPU_SPEED);//root->client->SystemInformation->CPU
    cpu_speed->SetText(m_cpu_speed);
    cpu_branch->InsertEndChild(cpu_speed);

    //end
    sys_info->InsertEndChild(cpu_branch);
    client->InsertEndChild(sys_info);
    root->InsertFirstChild(client);

    XMLPrinter printer;
    xml_doc.Accept(&printer);
    xml_str = printer.CStr();
}