#include "XML_Helpers.h"
#include <vector>
#include <stdexcept>
using namespace std;





list<string> explode(const string& str, char delimiter)
{
    list<string> result;

    size_t oldPos = 0;
    size_t pos = str.find_first_of(delimiter);
    while(pos != string::npos)
    {
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos+1;
        pos = str.find_first_of(delimiter, oldPos);
    }

    result.push_back(str.substr(oldPos, string::npos));

    return result;
}






vector<std::string> ioExplodev(const std::string& str, char delimiter)
{
    vector<string> result;

    size_t oldPos = 0;
    size_t pos = str.find_first_of(delimiter);
    while(pos != string::npos)
    {
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos+1;
        pos = str.find_first_of(delimiter, oldPos);
    }

    result.push_back(str.substr(oldPos, string::npos));

    // Test this:
    /*unsigned int pos;
    do
    {
        pos = str.find_first_of(delimiter, oldPos);
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos+1;
    }
    while(pos != string::npos);*/

    return result;
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


Point xmlGetPointAttr(TiXmlElement* elem, const string& attribute)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
    {
        printf("Failed to load attribute '%s' from '%s' element.\n", attribute.c_str(), elem->Value());
        return Point(0, 0);
    }
    return toPoint(attr);
}

Point xmlGetPointAttr(TiXmlElement* elem, const string& attribute, const Point& defaultValue)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
        return defaultValue;
    return toPoint(attr);
}


Pointf xmlGetPointfAttr(TiXmlElement* elem, const string& attribute)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
    {
        printf("Failed to load attribute '%s' from '%s' element.\n", attribute.c_str(), elem->Value());
        return Pointf(0, 0);
    }
    return toPointf(attr);
}

Pointf xmlGetPointfAttr(TiXmlElement* elem, const string& attribute, const Pointf& defaultValue)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
        return defaultValue;
    return toPointf(attr);
}


Color xmlGetColorAttr(TiXmlElement* elem, const string& attribute)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
    {
        printf("Failed to load attribute '%s' from '%s' element.\n", attribute.c_str(), elem->Value());
        return Color(0,0,0);
    }
    return toColor(attr);
}

Color xmlGetColorAttr(TiXmlElement* elem, const string& attribute, const Color& defaultValue)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
        return defaultValue;
    return toColor(attr);
}

Rect xmlGetRectAttr(TiXmlElement* elem, const string& attribute)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
    {
        printf("Failed to load attribute '%s' from '%s' element.\n", attribute.c_str(), elem->Value());
        return Rect(0, 0, 0, 0);
    }
    return toRect(attr);
}

Rect xmlGetRectAttr(TiXmlElement* elem, const string& attribute, const Rect& defaultValue)
{
    const char* attr = elem->Attribute(attribute.c_str());
    if(attr == NULL)
        return defaultValue;
    return toRect(attr);
}



