#include"XML_Parcer.h"


int main()
{
    XML_Parser a;
    string b = "config.xml";
    a.Read(b);
    outDocument c = a.GetData();//ready to use

    return 0;
}


