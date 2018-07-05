#include "stdafx.h"
#include "SettingUtils.h"
#include "base/CCUserDefault.h"
#include "platform/CCCommon.h"
#include "platform/CCFileUtils.h"

#include "base/base64.h"
#include "base/ccUtils.h"

#include "tinyxml2/tinyxml2.h"


using namespace std;
#define MAX_STR_VALUE 100

tinyxml2::XMLElement* SettingUtil::getXMLNodeForKey(const char* pKey, tinyxml2::XMLElement** rootNode, tinyxml2::XMLDocument** xmlDoc) {
    tinyxml2::XMLElement* curNode = nullptr;

    // check the key value
    if (!pKey) {
        return curNode;
    }

    if (nullptr == xmlDoc && nullptr == rootNode)
        return curNode;

    // get root node
    if (NULL == ( *xmlDoc ) && NULL == ( *rootNode )) {
        //CCLOG( "Err parse xml" );
        //return curNode;
        std::string xmlBuffer = _filePath.empty() ? _data : FileUtils::getInstance()->getStringFromFile(_filePath);

        if (xmlBuffer.empty()) {
            CCLOG("can not read xml file");
            return curNode;
        }
        *xmlDoc = new tinyxml2::XMLDocument();
        ( *xmlDoc )->Parse(xmlBuffer.c_str(), xmlBuffer.size());
    }

    tinyxml2::XMLElement* _rootNode;
    if (nullptr == xmlDoc || NULL == ( *xmlDoc )) {
        _rootNode = ( *rootNode );
    }
    else {
        _rootNode = ( *xmlDoc )->RootElement();
        if (nullptr != rootNode)
            ( *rootNode ) = _rootNode;
        if (NULL == _rootNode) {
            CCLOG("read root node error");
            return curNode;
        }
    }

    if (NULL != _rootNode) {
        const char* nodeName = _rootNode->Value();
        if (!strcmp(nodeName, pKey)) {
            return _rootNode;
        }
        //__
        curNode = _rootNode->FirstChildElement();
        while (nullptr != curNode) {
            const char* nodeName = curNode->Value();
            if (!strcmp(nodeName, pKey)) {
                break;
            }

            curNode = curNode->NextSiblingElement();
        }
    }

    return curNode;
}

tinyxml2::XMLElement* SettingUtil::getRootElement() {
    //ToDo
    const char* value = nullptr;
    tinyxml2::XMLElement* node;
    if (_rootName.empty())
        return NULL;

    node = getXMLNodeForKey(_rootName.c_str(), &_xmlRoot, &_doc);
    return node;
}

SettingUtil* SettingUtil::getSettingForKey(const char* pKey) {

    const char* value = nullptr;
    tinyxml2::XMLElement* node;

    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);

	SettingUtil* ret = new SettingUtil(false);
    ret->initXMLFile(node);

    //if (_minMemory)
    //	freeMemory();
    return ret;
}


bool SettingUtil::getBoolForKey(const char* pKey, bool defaultValue) {
    const char* value = nullptr;
    tinyxml2::XMLElement* node;
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // find the node
    if (node && node->FirstChild()) {
        value = (const char*) ( node->FirstChild()->Value() );
    }

    bool ret = defaultValue;

    if (value) {
        ret = ( !strcmp(value, "true") );
    }

    return ret;
}

int SettingUtil::getIntegerForKey(const char* pKey, int defaultValue) {
    const char* value = nullptr;
    tinyxml2::XMLElement* node;
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // find the node
    if (node && node->FirstChild()) {
        value = (const char*) ( node->FirstChild()->Value() );
    }

    int ret = defaultValue;

    if (value) {
        ret = atoi(value);
    }

    return ret;
}

float SettingUtil::getFloatForKey(const char* pKey, float defaultValue) {
    float ret = (float) getDoubleForKey(pKey, (double) defaultValue);

    return ret;
}

double SettingUtil::getDoubleForKey(const char* pKey, double defaultValue) {
    const char* value = nullptr;
    tinyxml2::XMLElement* node;
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // find the node
    if (node && node->FirstChild()) {
        value = (const char*) ( node->FirstChild()->Value() );
    }

    double ret = defaultValue;

    if (value) {
        ret = utils::atof(value);
    }

    return ret;
}

string SettingUtil::getStringForKey(const char* pKey, const std::string & defaultValue) {
    const char* value = nullptr;
    tinyxml2::XMLElement* node;
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // find the node
    if (node && node->FirstChild()) {
        value = (const char*) ( node->FirstChild()->Value() );
    }

    string ret = defaultValue;

    if (value) {
        ret = string(value);
    }

    return ret;
}

Color3B SettingUtil::getColor3BForKey(const char* pKey, const Color3B& defaultValue) {
    const char* value = nullptr;
    tinyxml2::XMLElement* node;
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // find the node
    if (node && node->FirstChild()) {
        value = (const char*) ( node->FirstChild()->Value() );
    }

    Color3B ret = defaultValue;

    if (value) {
        std::string val_s = string(value);
        int pos = val_s.find(';', 0);
        if (pos == 0)
            return ret;
        char tmp[20];
        memset(tmp, 0, 20);
        val_s.copy(tmp, pos, 0);
        int r = std::atoi(tmp);

        int pos_2 = val_s.find(';', pos + 1);
        if (pos_2 == 0)
            return ret;
        memset(tmp, 0, 20);
        val_s.copy(tmp, pos_2 - pos-1, pos + 1);
        int g = std::atoi(tmp);

        memset(tmp, 0, 20);
        val_s.copy(tmp, val_s.length() - pos_2 + 1, pos_2 + 1);
        int b = std::atoi(tmp);
        ret = Color3B(r, g, b);
    }

    return ret;
}


Vec3 SettingUtil::getVec3ForKey(const char* pKey, const Vec3& defaultValue) {
    const char* value = nullptr;
    tinyxml2::XMLElement* node;
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // find the node
    if (node && node->FirstChild()) {
        value = (const char*) ( node->FirstChild()->Value() );
    }

    Vec3 ret = defaultValue;

    if (value) {
        std::string val_s = string(value);
        size_t pos = val_s.find(';', 0);
        if (pos == 0)
            return ret;
        char tmp[20];
        memset(tmp, 0, 20);
        val_s.copy(tmp, pos - 1, 0);
        float x = std::atof(tmp);

        size_t pos_2 = val_s.find(';', pos + 1);
        if (pos_2 == 0)
            return ret;
        memset(tmp, 0, 20);
        val_s.copy(tmp, val_s.length() - pos_2 - 1, pos + 1);
        float y = std::atof(tmp);

        memset(tmp, 0, 20);
        val_s.copy(tmp, val_s.length() - pos_2 + 1, pos_2 + 1);
        float z = std::atof(tmp);

        ret = Vec3(x, y, z);
    }

    return ret;
}

Vec2 SettingUtil::getVec2ForKey(const char* pKey, const Vec2& defaultValue) {
    const char* value = nullptr;
    tinyxml2::XMLElement* node;
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // find the node
    if (node && node->FirstChild()) {
        value = (const char*) ( node->FirstChild()->Value() );
    }

    Vec2 ret = defaultValue;

    if (value) {
        std::string val_s = string(value);
        size_t pos = val_s.find(';', 0);
        if (pos == 0)
            return ret;
        char tmp[20];
        memset(tmp, 0, 20);
        val_s.copy(tmp, pos - 1, 0);
        float x = std::atof(tmp);
        memset(tmp, 0, 20);
        val_s.copy(tmp, val_s.length() - pos + 1, pos + 1);
        float y = std::atof(tmp);

        ret = Vec2(x, y);
    }

    return ret;
}

Data SettingUtil::getDataForKey(const char* pKey, const Data& defaultValue) {
    const char* encodedData = nullptr;
    tinyxml2::XMLElement* node;
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // find the node
    if (node && node->FirstChild()) {
        encodedData = (const char*) ( node->FirstChild()->Value() );
    }

    Data ret = defaultValue;

    if (encodedData) {
        unsigned char * decodedData = nullptr;
        int decodedDataLen = base64Decode((unsigned char*) encodedData, (unsigned int) strlen(encodedData), &decodedData);

        if (decodedData) {
            ret.fastSet(decodedData, decodedDataLen);
        }
    }

    return ret;
}

void SettingUtil::setValueForKey(const char* pKey, const char* pValue) {
    tinyxml2::XMLElement* node;
    // check the params
    if (!pKey || !pValue) {
        return;
    }
    // find the node
    node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // if node exist, change the content
    if (node) {
        if (node->FirstChild()) {
            node->FirstChild()->SetValue(pValue);
        }
        else {
            tinyxml2::XMLText* content = _doc->NewText(pValue);
            node->LinkEndChild(content);
        }
    }
    else {
        if (_xmlRoot) {
            tinyxml2::XMLElement* tmpNode = _doc->NewElement(pKey);//new tinyxml2::XMLElement(pKey);
            _xmlRoot->LinkEndChild(tmpNode);
            tinyxml2::XMLText* content = _doc->NewText(pValue);//new tinyxml2::XMLText(pValue);
            tmpNode->LinkEndChild(content);
        }
    }

    if (_autoSave)
        save();
}

void SettingUtil::setBoolForKey(const char* pKey, bool value) {
    // save bool value as string

    if (true == value) {
        setStringForKey(pKey, "true");
    }
    else {
        setStringForKey(pKey, "false");
    }
}

void SettingUtil::setIntegerForKey(const char* pKey, int value) {
    // check key
    if (!pKey) {
        return;
    }

    // format the value
    char tmp[MAX_STR_VALUE];
    memset(tmp, 0, MAX_STR_VALUE);
    sprintf(tmp, "%d", value);

    setValueForKey(pKey, tmp);
}

void SettingUtil::setFloatForKey(const char* pKey, float value) {
    setDoubleForKey(pKey, value);
}

void SettingUtil::setDoubleForKey(const char* pKey, double value) {
    // check key
    if (!pKey) {
        return;
    }

    // format the value
    char tmp[MAX_STR_VALUE];
    memset(tmp, 0, MAX_STR_VALUE);
    sprintf(tmp, "%f", value);

    setValueForKey(pKey, tmp);
}

void SettingUtil::setStringForKey(const char* pKey, const std::string & value) {
    // check key
    if (!pKey) {
        return;
    }

    setValueForKey(pKey, value.c_str());
}

void SettingUtil::setColor3BForKey(const char* pKey, const Color3B& value) {
    if (!pKey) {
        return;
    }

    // format the value
    char tmp[MAX_STR_VALUE];
    memset(tmp, 0, MAX_STR_VALUE);
    sprintf(tmp, "%i;%i;%i", (int) value.r, (int) value.g, (int) value.b);

    setValueForKey(pKey, tmp);
}

void SettingUtil::setVec3ForKey(const char* pKey, const Vec3& value) {
    if (!pKey) {
        return;
    }

    // format the value
    char tmp[MAX_STR_VALUE];
    memset(tmp, 0, MAX_STR_VALUE);
    sprintf(tmp, "%f;%f;%f", value.x, value.y, value.z);

    setValueForKey(pKey, tmp);
}

void SettingUtil::setVec2ForKey(const char* pKey, const Vec2& value) {
    if (!pKey) {
        return;
    }

    // format the value
    char tmp[MAX_STR_VALUE];
    memset(tmp, 0, MAX_STR_VALUE);
    sprintf(tmp, "%f;%f", value.x, value.y);

    setValueForKey(pKey, tmp);
}

void SettingUtil::setDataForKey(const char* pKey, const Data& value) {
    // check key
    if (!pKey) {
        return;
    }

    char *encodedData = 0;

    base64Encode(value.getBytes(), static_cast<unsigned int>( value.getSize() ), &encodedData);

    setValueForKey(pKey, encodedData);

    if (encodedData)
        free(encodedData);
}

void SettingUtil::setSettingForKey(const char* pKey, SettingUtil* value) {
    if (!pKey || NULL == value) {
        return;
    }
    tinyxml2::XMLElement* _rootNode = value->getRootElement();
    if (NULL == _rootNode)
        return;
    // find the node
    tinyxml2::XMLElement* node = getXMLNodeForKey(pKey, &_xmlRoot, &_doc);
    // if node exist, change the content
    bool _add = false;
    if (node) {
        node->DeleteChildren();
        _add = true;
    }
    else {
        if (_xmlRoot) {
            node = _doc->NewElement(pKey);//new tinyxml2::XMLElement(pKey);
            _xmlRoot->LinkEndChild(node);
            _add = true;
        }
    }
    if (_add) {
        tinyxml2::XMLElement* curNode = _rootNode->FirstChildElement();
        while (nullptr != curNode) {

            tinyxml2::XMLElement* line = _doc->NewElement(curNode->Value());//new tinyxml2::XMLElement(pKey);
            node->LinkEndChild(line);

            tinyxml2::XMLText* content = _doc->NewText(curNode->GetText());//new tinyxml2::XMLText(pValue);
            line->LinkEndChild(content);

            curNode = curNode->NextSiblingElement();
        }
    }

    if (_autoSave)
        save();
}
//#############################

SettingUtil::SettingUtil(bool init):
    _data(),
    _filePath(),
    _rootName() {
    isFile = false;
    isString = false;
    _doc = NULL;
    _xmlRoot = NULL;
    if (init)
        initXMLFile();
}


SettingUtil::~SettingUtil() {
    save();
    _data.clear();
    freeMemory();
}


#if IS_PC || IS_IOS
#define  LOGD(...) CCLOG(__VA_ARGS__)
#elif IS_ANDROID
#include "platform/CCPlatformConfig.h"
#define  LOG_TAG    "CCFileUtils-stage"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif

SettingUtil::SettingUtil(const std::string& fileName, bool autoSave):
    _data(),
    _filePath(),
    _rootName() {
    isFile = true;
    isString = false;
    _doc = NULL;
    _xmlRoot = NULL;
    _autoSave = autoSave;
    _filePath = fileName;
    {

#if 0//1//def CC_PLATFORM_WIN32
        const std::vector<std::string> path = FileUtils::getInstance()->getSearchPaths();
        size_t pos = MAX(path.size() - 1, 0);
        _filePath = path[pos] + fileName;
#endif
        if (!FileUtils::getInstance()->isFileExist(_filePath)) {
            //_filePath += FileUtils::getInstance()->getWritablePath() + fileName;
#if HAS_DEVELOP
            _filePath = HelperUtils::getLocalPath(false) + _filePath;
#else
            DebugBreak();
#endif
        }

		_data = FileUtils::getInstance()->getStringFromFile(_filePath);
        if (_data.empty())
            initXMLFile();
    }
}

void SettingUtil::setData(const std::string& data) {
    isString = true;
    _data = std::string(data);
    if (isFile) {
        _filePath.clear();
        isFile = false;
    }
    initXMLFile();
};

void SettingUtil::initXMLFile(tinyxml2::XMLElement* xmlRoot) {
    isFile = false;
    isString = false;
    _xmlRoot = xmlRoot;
}

void SettingUtil::initXMLFile(const std::string& rootName, bool hasSave) {
    if (NULL != _doc) {
        delete _doc; _doc = NULL;
    }

    _doc = new tinyxml2::XMLDocument();

    tinyxml2::XMLDeclaration *pDeclaration = _doc->NewDeclaration(nullptr);

    _doc->LinkEndChild(pDeclaration);

    _rootName = rootName.empty() ? USERDEFAULT_ROOT_NAME : std::string(rootName);

    _xmlRoot = _doc->NewElement(_rootName.c_str());

    _doc->LinkEndChild(_xmlRoot);
    if (hasSave)
        save();
}

void SettingUtil::save() {
    if (NULL == _doc)
        return;
    if (isString) {
        _data.clear();
        tinyxml2::XMLPrinter stdStreamer;
        _doc->Print(&stdStreamer);
        _data = string(stdStreamer.CStr());
    }
    else if (isFile) {
        bool bRet = tinyxml2::XML_SUCCESS == _doc->SaveFile(FileUtils::getInstance()->getSuitableFOpen(_filePath).c_str());
    }

}

void SettingUtil::freeMemory() {
    if (NULL != _doc) {
        delete _doc; _doc = NULL;
        _xmlRoot = NULL;
    }
}

void SettingUtil::clear() {
    initXMLFile(_rootName);
}

void SettingUtil::deleteValueForKey(const char* key) {
    tinyxml2::XMLElement* node;

    // check the params
    if (!key) {
        CCLOG("the key is invalid");
        return;
    }

    // find the node
    node = getXMLNodeForKey(key, &_xmlRoot, &_doc);

    // if node not exist, don't need to delete
    if (!node) {
        return;
    }

    // save file and free doc
    if (NULL != _doc) {
        _doc->DeleteNode(node);
        save();
    }
}
