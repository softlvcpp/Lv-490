#pragma once
#include"pch.h"
#include"XMLClient.h"


void XMLClient::WriteSystemInformation(string& xml_str) const noexcept
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

    //HardDisk loop
    XMLElement* hard_disk;
    for (size_t i = 0; i < m_hard_disk_type_list.size(); ++i)
    {
        hard_disk = xml_doc.NewElement(CLIENT_HARDDISK);//root->client->SystemInformation->HardDisk

        hard_disk->SetAttribute(CLIENT_TYPE, m_hard_disk_type_list[i].c_str());

        hard_disk->SetAttribute(CLIENT_DRIVE, m_hard_disk_type_list[i].c_str());

        XMLElement* pHardDisk_TotalSize = xml_doc.NewElement(CLIENT_HARDDISK_TOTALSIZE);//root->client->SystemInformation->HardDisk
        pHardDisk_TotalSize->SetText(m_hard_disk_total_size[i]);
        XMLElement* pHardDisk_Used = xml_doc.NewElement(CLIENT_HARDDISK_USED);//root->client->SystemInformation->HardDisk
        pHardDisk_Used->SetText(m_hard_disk_used[i]);
        XMLElement* pHardDisk_Free = xml_doc.NewElement(CLIENT_HARDDISK_FREE);//root->client->SystemInformation->HardDisk
        pHardDisk_Free->SetText(m_hard_disk_free[i]);

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
    sys_info->InsertEndChild(cpu_branch);

    XMLElement* process_info = xml_doc.NewElement(CLIENT_PROCESSINFORMATION);//root->client->SystemInformation->ProcessInformation

    XMLElement* process;

    for (auto it = m_processes.begin(); it != m_processes.end(); it++)//map iterator
    {
        process = xml_doc.NewElement(CLIENT_PROCESS);
        process->SetAttribute(CLIENT_PROCESS_ATTRIBUTE_NAME, it->second.c_str());
        process->SetAttribute(CLIENT_PROCESS_ATTRIBUTE_PID, it->first);
        process_info->InsertEndChild(process);
    }
    sys_info->InsertEndChild(process_info);

    //end    
    client->InsertEndChild(sys_info);
    root->InsertFirstChild(client);

    XMLPrinter printer;
    xml_doc.Accept(&printer);
    xml_str = printer.CStr();
}