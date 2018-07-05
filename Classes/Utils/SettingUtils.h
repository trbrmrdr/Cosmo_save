#pragma once
#include "platform/CCPlatformMacros.h"
#include <string>
#include "base/CCData.h"
#include "external/tinyxml2/tinyxml2.h"
#include "math/Vec2.h"

#define USERDEFAULT_ROOT_NAME    "userDefaultRoot"

class  SettingUtil
{
public:

	void setValueForKey(const char* pKey, const char* pValue);
	tinyxml2::XMLElement* getXMLNodeForKey(const char* pKey, tinyxml2::XMLElement** rootNode, tinyxml2::XMLDocument** doc);

	bool getBoolForKey(const char* key, bool defaultValue = false);

	int getIntegerForKey(const char* key, int defaultValue = -1);

	float getFloatForKey(const char* key, float defaultValue = 0);

	double getDoubleForKey(const char* key, double defaultValue = 0.0);

	std::string getStringForKey(const char* key, const std::string& defaultValue = "");

	tinyxml2::XMLElement* getRootElement();
	SettingUtil* getSettingForKey(const char* pKey);

	Data getDataForKey(const char* key, const Data& defaultValue = Data::Null);

	Vec2 getVec2ForKey(const char* key, const Vec2& defaultValue = Vec2::ZERO);

	Vec3 getVec3ForKey(const char* key, const Vec3& defaultValue = Vec3::ZERO);

	Color3B getColor3BForKey(const char* key, const Color3B& defaultValue = Color3B::BLACK);


	void setBoolForKey(const char* key, bool value);

	void setIntegerForKey(const char* key, int value);

	void setFloatForKey(const char* key, float value);

	void setDoubleForKey(const char* key, double value);

	void setStringForKey(const char* key, const std::string & value);

	void setDataForKey(const char* key, const Data& value);

	void setVec2ForKey(const char* key, const Vec2& value);

	void setVec3ForKey(const char* key, const Vec3& value);

	void setColor3BForKey(const char* key, const Color3B& value);

	void setSettingForKey(const char* key, SettingUtil* value);

	void deleteValueForKey(const char* key);

	void clear();

	//minMemory -> auto clear xml after Saves
	static SettingUtil* createIsFile(const std::string& fileName, bool autoSave = true) { return new SettingUtil(fileName, autoSave); };

	SettingUtil(bool init);

	SettingUtil(const std::string& fileName, bool autoSave = true);


	~SettingUtil();

	void setData(const std::string& data);

	std::string getData() { return _data; };

	void initXMLFile(tinyxml2::XMLElement* xmlRoot);

	void initXMLFile(const std::string& rootName = USERDEFAULT_ROOT_NAME, bool save = true);

	void save();
private:

	void freeMemory();

	bool isFile;
	std::string _filePath;

	bool isString;
	std::string _data;

	tinyxml2::XMLDocument* _doc;
	tinyxml2::XMLElement* _xmlRoot;

	bool _autoSave;

	std::string _rootName;

};
