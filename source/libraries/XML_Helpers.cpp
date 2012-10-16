#include "XML_Helpers.h"
#include <vector>
#include <stdexcept>
using namespace std;



std::string toLower(const std::string& str)
{
    std::string result = str;
    for(unsigned int i = 0; i < result.size(); i++)
    {
        result[i] = tolower(result[i]);
    }
    return result;
}


bool toBool(const std::string& str)
{
    if(toLower(str) == "true")
        return true;
    if(toLower(str) == "false")
        return false;
    return atoi(str.c_str());
}

int toInt(const std::string& str)
{
    return atoi(str.c_str());
}

float toFloat(const std::string& str)
{
    return atof(str.c_str());
}



std::string toString(bool b)
{
    return (b? "true" : "false");
}

std::string toString(int n)
{
    char buf[20];
    sprintf(buf, "%d", n);
    return buf;
}

char* stripTrailingDecimalZeros(char* buf)
{
    if(buf == NULL)
        return NULL;
    
    char* c = buf;
    
    // See if it has a decimal
    bool hasDecimal = false;
    while(*c != '\0')
    {
        if(*c == '.')
            hasDecimal = true;
        c++;
    }
    
    // If there is a decimal part, remove all the trailing zeros and the decimal point if we reach it.
    if(hasDecimal)
    {
        while(c != buf)
        {
            c--;
            if(*c == '0')
                *c = '\0';
            else
            {
                if(*c == '.')
                    *c = '\0';
                break;
            }
        }
    }
    
    return buf;
}

std::string toString(float f, int precision)
{
    char buf[20];
    if(precision >= 0)
        snprintf(buf, 20, "%.*f", precision, f);
    else
    {
        snprintf(buf, 20, "%f", f);
        stripTrailingDecimalZeros(buf);
    }
    return buf;
}












bool xmlAttrExists(TiXmlElement* elem, const string& attribute)
{
    return (elem->Attribute(attribute.c_str()) != NULL);
}

string xmlGetStringAttr(TiXmlElement* elem, const string& attribute)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
    {
        printf("Failed to load attribute '%s' from '%s' element.\n", attribute.c_str(), elem->Value());
        return "";
    }
    return attr;
}

string xmlGetStringAttr(TiXmlElement* elem, const string& attribute, const string& defaultValue)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
        return defaultValue;
    return attr;
}

bool xmlGetBoolAttr(TiXmlElement* elem, const string& attribute)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
    {
        printf("Failed to load attribute '%s' from '%s' element.\n", attribute.c_str(), elem->Value());
        return false;
    }
    return toBool(attr);
}

bool xmlGetBoolAttr(TiXmlElement* elem, const string& attribute, bool defaultValue)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
        return defaultValue;
    return toBool(attr);
}

int xmlGetIntAttr(TiXmlElement* elem, const string& attribute)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
    {
        printf("Failed to load attribute '%s' from '%s' element.\n", attribute.c_str(), elem->Value());
        return 0;
    }
    return toInt(attr);
}

int xmlGetIntAttr(TiXmlElement* elem, const string& attribute, int defaultValue)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
        return defaultValue;
    return toInt(attr);
}

float xmlGetFloatAttr(TiXmlElement* elem, const string& attribute)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
    {
        printf("Failed to load attribute '%s' from '%s' element.\n", attribute.c_str(), elem->Value());
        return 0.0f;
    }
    return toFloat(attr);
}


float xmlGetFloatAttr(TiXmlElement* elem, const string& attribute, float defaultValue)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
        return defaultValue;
    return toFloat(attr);
}



