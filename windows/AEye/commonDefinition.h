#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>

typedef std::pair<std::string, std::string>	FilesPair;
typedef std::map<std::string, std::string>	FilesMap;

typedef std::pair<float, std::string>	Top1Pair;
typedef std::map<float, std::string, std::greater<float> >	Top1Map;

typedef std::pair<std::string, Top1Map>	ClassTop1Pair;
typedef std::map<std::string, Top1Map>	ClassTop1Map;

enum ID_PROPERTY_DATA
{
	ID_PROPERTY_CLASS_EMPTY,	//	0
	ID_PROPERTY_CLASS1_NAME,	//	1
	ID_PROPERTY_CLASS1_RATIO,	//	2
	ID_PROPERTY_CLASS2_NAME,	//	
	ID_PROPERTY_CLASS2_RATIO,	//	4
	ID_PROPERTY_CLASS3_NAME,	//	
	ID_PROPERTY_CLASS3_RATIO,	//	
	ID_PROPERTY_CLASS4_NAME,	//	
	ID_PROPERTY_CLASS4_RATIO,	//	8
	ID_PROPERTY_CLASS5_NAME,	//	
	ID_PROPERTY_CLASS5_RATIO,	//	

	ID_PROPERTY_COUNT			//	11
};

enum LOG_TYPE
{
	LOG_TYPE_UI_OUTPUT = 1,		// 001
	LOG_TYPE_UI_PROPERTY = 2,	// 010
	LOG_TYPE_UI_FILE = 4,		// 100
	LOG_TYPE_UI_VIEW = 8,		// 1000
	LOG_TYPE_UI_ALL = LOG_TYPE_UI_OUTPUT | LOG_TYPE_UI_PROPERTY | LOG_TYPE_UI_FILE | LOG_TYPE_UI_VIEW		// 1111
};
