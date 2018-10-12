
#include "stdafx.h"
#include "Utility.h"


string UtilityFile::getShortFileName(string& fullpath)
{
	int indexEnd = fullpath.find_last_of('\\');
	return fullpath.substr(indexEnd + 1);
}

string UtilityFile::getPathFileName(string& fullpath)
{
	int indexEnd = fullpath.find_last_of('\\');
	return fullpath.substr(0, indexEnd + 1);
}
