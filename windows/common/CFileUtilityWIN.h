
#pragma once
#include "commonDefinitionFile.h"

class CFileUtilityWIN
{
public:
	CFileUtilityWIN(){}
	~CFileUtilityWIN(){}

public:
	static void getFileListFromPath(tstring &path, tstring fmt, std::vector<tstring> &list);
	
	static void getFileListFromPathNest(tstring path, tstring pathSub, StringVec &fmts, StringVec &list);

	static void getFilePathFromDialog(tstring &path);

	static void getSubPathFromPath(tstring &path, std::vector<tstring> &list);

	static bool createPath(tstring &path);

	static bool createPath(tstring &path, StringIDMap &pathList);

	static bool		getPathExist(tstring& path);

	static bool removePath(tstring &path);

};