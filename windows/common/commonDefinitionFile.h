
#pragma once 

#include <functional>
#include <algorithm>

#include <vector>
#include <string>
#include <map>

#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

#ifdef _UNICODE
#define tstring std::wstring
#define tsstream std::wostringstream
#define tstrstr	std::wcsstr
#define tstrcmp	std::wcscmp
#define tfstream std::wfstream

#else
#define tstring std::string
#define tsstream std::ostringstream
#define tstrstr	std::strstr
#define tstrcmp	std::strcmp
#define tfstream std::fstream

#endif

typedef std::vector<tstring>	StringVec;

typedef std::pair<tstring, int>	StringIDPair;
typedef std::map<tstring, int>	StringIDMap;

typedef std::pair<tstring, tstring>	FilesPair;
typedef std::map<tstring, tstring>	FilesMap;

