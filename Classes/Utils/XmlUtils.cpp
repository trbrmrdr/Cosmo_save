#pragma once
#include "cocos2d.h"
#include "XmlUtils.h"
#include "tinyxml2/tinyxml2.h"


std::string xmlAttrToString(const XMLElement* node, const std::string& name, const std::string& def) {
	if (!node || name.empty()) return def;
	const char* val = node->Attribute(name.c_str());
	return val ? val : def;
}

int xmlAttrToInt(const XMLElement* node, const std::string& name, const int def) {
	if (!node || name.empty()) return def;
	const char* val = node->Attribute(name.c_str());
	return val ? atoi(val) : def;
}

bool xmlAttrToBool(const XMLElement* node, const std::string& name, const bool def) {
	return xmlAttrToInt(node, name, (bool)def) != 0;
}

float xmlAttrToFloat(const XMLElement* node, const std::string& name, const float def) {
	if (!node || name.empty()) return def;
	const char* val = node->Attribute(name.c_str());
	return val ? atof(val) : def;
}

std::string xmlToString(const XMLElement* node, const std::string& def) {
	if (!node) return def;
	const char* val = node->GetText();
	return val ? val : def;
}

int xmlToInt(const XMLElement* node, const int def) {
	if (!node) return def;
	const char* val = node->GetText();
	return val ? atof(val) : def;
}

bool xmlToBool(const XMLElement* node, const bool def) {
	return xmlToInt(node, (bool)def) != 0;
}

float xmlToFloat(const XMLElement* node, const float def) {
	if (!node) return def;
	const char* val = node->GetText();
	return val ? atof(val) : def;
}
