
#pragma once
#include "commonDefinitionFile.h"
#include "stdio.h"

class CFileUtilitySTL
{
public:
	CFileUtilitySTL(){}
	~CFileUtilitySTL(){}

public:
	static tstring getShortFileName(tstring& fullpath);

	static tstring getPathFileName(tstring& fullpath);

	static void		writeFilelist(tstring filename, FilesMap& filesMap, int index = -1, bool rewrite = true);

	static bool		readFilelist(tstring filename, StringVec& lines);
	static bool		writeFilelist(tstring filename, StringVec& lines);

	static bool		readFilelist(tstring filename, StringIDMap& lines);
	static bool		writeFilelist(tstring filename, StringIDMap& lines);

	static void		convertList2Map(StringIDMap& classes, StringVec& lines);

	// return 0, success 
	static int		removeFile(tstring filename);

	static bool		generateVal(tstring filename, tstring filenameVal);

	static bool		copyFile(tstring& fromPath, tstring& toPath);

	static bool		copyFilelist(tstring& fromPath, tstring& toPath, StringIDMap& lines);

	static bool		copyFilelistRename(tstring& fromPath, tstring& toPath, StringIDMap& lines);

	static tstring	unfoldRelativePath(tstring path);

	static StringVec split(const tstring &str, const tstring &pattern);

	static bool		checkFileExist(tstring& filepath);

	static tstring		getFileExt(tstring& filepath);

};