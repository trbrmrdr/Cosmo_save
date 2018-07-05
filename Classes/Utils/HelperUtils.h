#pragma once

#include "cocos2d.h"
USING_NS_CC;


namespace HelperUtils {
#if HAS_DEVELOP
    long GetLastDataEdit(const char* fileName);
    std::string getLocalPath(bool extended);
#endif
    double GetCurrTime();
    int fixPOT(int x);

	std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty = true);
}
