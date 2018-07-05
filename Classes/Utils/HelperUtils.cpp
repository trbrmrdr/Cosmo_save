#include "stdafx.h"
#include "HelperUtils.h"

#if IS_PC || IS_IOS
#define  LOGD(...) CCLOG(__VA_ARGS__)
#elif IS_ANDROID
#include "platform/CCPlatformConfig.h"
#define  LOG_TAG    "Brush"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif

#if HAS_DEVELOP
#include "platform/CCFileUtils.h"
#include "platform/CCPlatformMacros.h"
#include "platform/win32/CCUtils-win32.h"
#include <ShlObj.h>
#define CC_MAX_PATH  512

using namespace std;


static std::string _writableExtendedPath = "";
static std::string _writablePath = "";
std::string HelperUtils::getLocalPath(bool extended) {
	if (extended && _writableExtendedPath.length())
		return _writableExtendedPath;
	if (_writablePath.length())
		return _writablePath;

	// Get full path of executable, e.g. c:\Program Files (x86)\My Game Folder\MyGame.exe
	WCHAR full_path[CC_MAX_PATH + 1] = { 0 };
	::GetModuleFileName(nullptr, full_path, CC_MAX_PATH + 1);

	// Debug app uses executable directory; Non-debug app uses local app data directory
	//#ifndef _DEBUG
	// Get filename of executable only, e.g. MyGame.exe
	wstring retPath;
	if (extended) {
		WCHAR *base_name = wcsrchr(full_path, '\\');


		if (base_name) {
			WCHAR app_data_path[CC_MAX_PATH + 1];

			// Get local app data directory, e.g. C:\Documents and Settings\username\Local Settings\Application Data
			if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, app_data_path))) {
				wstring ret(app_data_path);

				// Adding executable filename, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame.exe
				ret += base_name;

				// Remove ".exe" extension, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame
				ret = ret.substr(0, ret.rfind(L"."));

				ret += L"\\";

				// Create directory
				if (SUCCEEDED(SHCreateDirectoryEx(nullptr, ret.c_str(), nullptr))) {
					retPath = ret;
				}
			}
		}
	}
	if (!extended && retPath.empty())
		//#endif // not defined _DEBUG
	{
		// If fetching of local app data directory fails, use the executable one
		retPath = full_path;

		// remove xxx.exe
		retPath = retPath.substr(0, retPath.rfind(L"\\") + 1);
	}

	//std::string ret = convertPathFormatToUnixStyle(StringWideCharToUtf8(retPath));
	//############
	//static inline std::string convertPathFormatToUnixStyle(const std::string& path) {
	std::string ret = StringWideCharToUtf8(retPath);// path;
	int len = ret.length();
	for (int i = 0; i < len; ++i) {
		if (ret[i] == '\\') {
			ret[i] = '/';
		}
	}
	//return ret;}
	//###########
	if (extended)
		_writableExtendedPath = ret;
	else
		_writablePath = ret;

	return ret;
}

long HelperUtils::GetLastDataEdit(const char* fileName) {
	struct tm* clock;
	struct stat attrib;
	stat(fileName, &attrib);
	clock = gmtime(&(attrib.st_mtime));
	return  clock->tm_hour * 10000
		+ clock->tm_min * 100
		+ clock->tm_sec;
}
#endif


double HelperUtils::GetCurrTime() {//in sec
	struct timeval t;
	gettimeofday(&t, NULL);
	//float retMs = (double) t.tv_sec * 1000 + (double) t.tv_usec / 1000.0;
	double retS = (double)t.tv_sec + (double)t.tv_usec / 1000000.0;
	//return (float) utils::getTimeInMilliseconds()
	//LOGD("rets = %.2f", (float) retS);
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#endif
	return retS;

	/*
	struct timeval now;
	if (gettimeofday(&now, nullptr)!=0)
	{
		CCLOG("error in gettimeofday");
		return 0;
	}
	return (now.tv_sec)+(now.tv_usec)/1000000.0f;
	*/
}


int HelperUtils::fixPOT(int x) {
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}

namespace HelperUtils {

	using namespace std;
	std::vector<string> split(const string& s, const string& delim, const bool keep_empty/*= true*/) {
		std::vector<string> result;
		if (delim.empty()) {
			result.push_back(s);
			return result;
		}
		std::string::const_iterator substart = s.begin(), subend;
		while (true) {
			subend = search(substart, s.end(), delim.begin(), delim.end());
			string temp(substart, subend);
			if (keep_empty || !temp.empty()) {
				result.push_back(temp);
			}
			if (subend == s.end()) {
				break;
			}
			substart = subend + delim.size();
		}
		return result;
	}
}
