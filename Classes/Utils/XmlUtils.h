#ifndef HEADER_XmlUtils_h
#define HEADER_XmlUtils_h


#include "tinyxml2/tinyxml2.h"
#include <string>

using namespace tinyxml2;

#define FOR_EACH_XML_ELEMENT(root, node, element) \
    for (const XMLElement *node = root->FirstChildElement(element); node != NULL; node = node->NextSiblingElement(element))

std::string xmlAttrToString(const tinyxml2::XMLElement* node, const std::string& name, const std::string& def);
int xmlAttrToInt(const tinyxml2::XMLElement* node, const std::string& name, const int def);
bool xmlAttrToBool(const tinyxml2::XMLElement* node, const std::string& name, const bool def);
float xmlAttrToFloat(const tinyxml2::XMLElement* node, const std::string& name, const float def);

std::string xmlToString(const tinyxml2::XMLElement* node, const std::string& def);
int xmlToInt(const tinyxml2::XMLElement* node, const int def);
bool xmlToBool(const tinyxml2::XMLElement* node, const bool def);
float xmlToFloat(const tinyxml2::XMLElement* node, const float def);



#endif // HEADER_XmlUtils_h

