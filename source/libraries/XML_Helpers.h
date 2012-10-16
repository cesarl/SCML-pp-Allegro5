#ifndef _XML_HELPERS_H__
#define _XML_HELPERS_H__


#include "SDL_Helpers.h"
#include "tinyxml.h"
#include <string>
#include <list>

std::list<std::string> explode(const std::string& str, char delimiter);

bool toBool(const std::string& str);
int toInt(const std::string& str);
float toFloat(const std::string& str);
Rect toRect(const std::string& str);
Color toColor(const std::string& str);
Point toPoint(const std::string& str);
Pointf toPointf(const std::string& str);


std::string getInsideParens(const std::string& s);


bool xmlGetBoolAttr(TiXmlElement* elem, const std::string& attribute);
bool xmlGetBoolAttr(TiXmlElement* elem, const std::string& attribute, bool defaultValue);
std::string xmlGetStringAttr(TiXmlElement* elem, const std::string& attribute);
std::string xmlGetStringAttr(TiXmlElement* elem, const std::string& attribute, const std::string& default_value);
int xmlGetIntAttr(TiXmlElement* elem, const std::string& attribute);
int xmlGetIntAttr(TiXmlElement* elem, const std::string& attribute, int default_value);
float xmlGetFloatAttr(TiXmlElement* elem, const std::string& attribute);
float xmlGetFloatAttr(TiXmlElement* elem, const std::string& attribute, float default_value);
Point xmlGetPointAttr(TiXmlElement* elem, const std::string& attribute);
Point xmlGetPointAttr(TiXmlElement* elem, const std::string& attribute, const Point& defaultValue);
Pointf xmlGetPointfAttr(TiXmlElement* elem, const std::string& attribute);
Pointf xmlGetPointfAttr(TiXmlElement* elem, const std::string& attribute, const Pointf& defaultValue);
Color xmlGetColorAttr(TiXmlElement* elem, const std::string& attribute);
Color xmlGetColorAttr(TiXmlElement* elem, const std::string& attribute, const Color& defaultValue);
Rect xmlGetRectAttr(TiXmlElement* elem, const std::string& attribute);
Rect xmlGetRectAttr(TiXmlElement* elem, const std::string& attribute, const Rect& defaultValue);



#endif
