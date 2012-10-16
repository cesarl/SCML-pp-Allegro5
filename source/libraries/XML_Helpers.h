#ifndef _XML_HELPERS_H__
#define _XML_HELPERS_H__


#include "tinyxml.h"
#include <string>

bool toBool(const std::string& str);
int toInt(const std::string& str);
float toFloat(const std::string& str);

std::string toString(bool b);
std::string toString(int n);
std::string toString(float f, int precision = -1);


bool xmlGetBoolAttr(TiXmlElement* elem, const std::string& attribute);
bool xmlGetBoolAttr(TiXmlElement* elem, const std::string& attribute, bool defaultValue);
std::string xmlGetStringAttr(TiXmlElement* elem, const std::string& attribute);
std::string xmlGetStringAttr(TiXmlElement* elem, const std::string& attribute, const std::string& default_value);
int xmlGetIntAttr(TiXmlElement* elem, const std::string& attribute);
int xmlGetIntAttr(TiXmlElement* elem, const std::string& attribute, int default_value);
float xmlGetFloatAttr(TiXmlElement* elem, const std::string& attribute);
float xmlGetFloatAttr(TiXmlElement* elem, const std::string& attribute, float default_value);



#endif
