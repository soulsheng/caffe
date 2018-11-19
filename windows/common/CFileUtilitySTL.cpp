#include "stdafx.h"
#include "CFileUtilitySTL.h"

tstring CFileUtilitySTL::getShortFileName(tstring& fullpath)
{
	int indexEnd = fullpath.find_last_of('\\');
	return fullpath.substr(indexEnd + 1);
}

tstring CFileUtilitySTL::getPathFileName(tstring& fullpath)
{
	int indexEnd = fullpath.find_last_of(_T("\\/"));
	return fullpath.substr(0, indexEnd + 1);
}

void CFileUtilitySTL::writeFilelist(tstring filename, FilesMap& filesMap, int index/* = -1*/, bool rewrite/* = true*/)
{
	std::locale oNewLocale(std::locale(), "", std::locale::ctype);
	std::locale oPreviousLocale = std::locale::global(oNewLocale);

	tfstream fileOut;
		
	if (rewrite)
		fileOut.open(filename, ios::out);
	else
		fileOut.open(filename, ios::out | ios ::app );

	for (FilesMap::iterator itr = filesMap.begin(); itr != filesMap.end(); itr++)
	{
		if (index==-1)
			fileOut << itr->first << std::endl;
		else
			fileOut << itr->first << _T(" ") << index << std::endl;

	}

	fileOut.close();
	

	std::locale::global(oPreviousLocale);

}

bool CFileUtilitySTL::writeFilelist(tstring filename, StringVec& lines)
{
	std::locale oNewLocale(std::locale(), "", std::locale::ctype);
	std::locale oPreviousLocale = std::locale::global(oNewLocale);

	tfstream fileOut;

	fileOut.open(filename, ios::out);
	if (!fileOut)
	{//���û�ɹ�
		return false;
	}

	for (StringVec::iterator itr = lines.begin(); itr != lines.end(); itr++)
		fileOut << *itr << std::endl;

	fileOut.close();

	std::locale::global(oPreviousLocale);

	return true;
}

bool CFileUtilitySTL::writeFilelist(tstring filename, StringIDMap& lines)
{
	std::locale oNewLocale(std::locale(), "", std::locale::ctype);
	std::locale oPreviousLocale = std::locale::global(oNewLocale);

	tfstream fileOut;

	fileOut.open(filename, ios::out);
	if (!fileOut)
	{//���û�ɹ�
		return false;
	}

	for (StringIDMap::iterator itr = lines.begin(); itr != lines.end(); itr++)
		fileOut << itr->first << _T(" ") << itr->second << std::endl;

	fileOut.close();

	std::locale::global(oPreviousLocale);

	return true;
}

bool CFileUtilitySTL::readFilelist(tstring filename, StringVec& lines)
{
	std::locale oNewLocale(std::locale(), "", std::locale::ctype);
	std::locale oPreviousLocale = std::locale::global(oNewLocale);

	tfstream file(filename);

	//����ļ��Ƿ�򿪳ɹ�
	if (!file)
	{//���û�ɹ�

		return false;
	}
	else
	{//�ļ��򿪳ɹ�����ʼ���ж��ļ�����
		tstring s;
		//fstream����Ҳ��getline��Ա��������ҪŪ��
		//getline(infile,s);
		while (!file.eof())
		{
			//infile >> s;
			getline(file, s);
			lines.push_back(s);
		}
	}
	file.close();

	std::locale::global(oPreviousLocale);

	return true;
}

bool CFileUtilitySTL::readFilelist(tstring filename, StringIDMap& lines)
{
	std::locale oNewLocale(std::locale(), "", std::locale::ctype);
	std::locale oPreviousLocale = std::locale::global(oNewLocale);

	tfstream file;

	file.open(filename, ios::in);
	if (!file)
	{//���û�ɹ�
		return false;
	}

	tstring s;
	//fstream����Ҳ��getline��Ա��������ҪŪ��
	//getline(infile,s);
	while (!file.eof())
	{
		//infile >> s;
		getline(file, s);

		int indexBlank = s.find_last_of(_T(" "));
		if( -1 == indexBlank)
			continue;
		tstring lastID = s.substr(indexBlank);
		int id = _ttoi(lastID.c_str());
		lines.insert(StringIDPair(s.substr(0, indexBlank), id));
	}

	file.close();

	std::locale::global(oPreviousLocale);

	return true;
}

void CFileUtilitySTL::convertList2Map(StringIDMap& classes, StringVec& lines)
{
	int index = 0;
	for (StringVec::iterator itr = lines.begin(); itr != lines.end();itr++)
		classes.insert(StringIDPair(*itr, index++));
	
}

int CFileUtilitySTL::removeFile(tstring filename)
{
	return _tremove(filename.c_str());
}

bool CFileUtilitySTL::generateVal(tstring filename, tstring filenameVal)
{
	std::locale oNewLocale(std::locale(), "", std::locale::ctype);
	std::locale oPreviousLocale = std::locale::global(oNewLocale);

	tfstream file(filename);

	StringVec lines;

	//����ļ��Ƿ�򿪳ɹ�
	if (!file)
	{//���û�ɹ�

		throw runtime_error("file cannot open");
		return false;
	}
	else
	{//�ļ��򿪳ɹ�����ʼ���ж��ļ�����
		tstring s;
		//fstream����Ҳ��getline��Ա��������ҪŪ��
		//getline(infile,s);
		while (!file.eof())
		{
			getline(file, s);
			if (s.length()>4)// ignore empty line
				lines.push_back(s);
		}
	}
	file.close();

	std::srand(unsigned(time(0))); 
	random_shuffle(lines.begin(), lines.end());

	tfstream fileVal(filenameVal, ios::out);

	for (StringVec::iterator itr = lines.begin(); itr != lines.end(); itr++)
		fileVal << *itr << std::endl;

	fileVal.close();

	std::locale::global(oPreviousLocale);

	return true;
}

bool CFileUtilitySTL::copyFilelist(tstring& fromPath, tstring& toPath, StringIDMap& lines)
{
	StringIDMap::iterator itr = lines.begin();

	for (; itr!=lines.end(); itr++)
		copyFile(fromPath + itr->first, toPath + itr->first);

	return true;
}

bool CFileUtilitySTL::copyFilelistRename(tstring& fromPath, tstring& toPath, StringIDMap& lines)
{
	StringIDMap::iterator itr = lines.begin();

	for (; itr != lines.end(); itr++)
	{
		tstring filenameNew = unfoldRelativePath(itr->first);

		copyFile(fromPath + itr->first, toPath + filenameNew);

	}

	return true;
}

tstring CFileUtilitySTL::unfoldRelativePath(tstring path)
{
	StringVec resultVec = CFileUtilitySTL::split(path, _T("\\/"));

	tstring pathNew;
	for (StringVec::iterator itr = resultVec.begin(); itr != resultVec.end(); itr++)
	{
		pathNew += _T("-");
		pathNew += *itr;
	}
	return pathNew;
}

bool CFileUtilitySTL::copyFile(tstring& fromPath, tstring& toPath)
{
	// ref: https://www.cnblogs.com/endenvor/p/6819043.html 
	using namespace std;
	ifstream in(fromPath, ios::binary);
	ofstream out(toPath, ios::binary);
	if (!in.is_open()) {
		return false;
	}
	if (!out.is_open()) {
		return false;
	}
	if (fromPath == toPath) {	
		return false;
	}
	char buf[2048];
	long long totalBytes = 0;
	while (in)
	{
		//read��in���ж�ȡ2048�ֽڣ�����buf�����У�ͬʱ�ļ�ָ������ƶ�2048�ֽ�
		//������2048�ֽ������ļ���β������ʵ����ȡ�ֽڶ�ȡ��
		in.read(buf, 2048);
		//gcount()������ȡ��ȡ���ֽ�����write��buf�е�����д��out����
		out.write(buf, in.gcount());
		totalBytes += in.gcount();
	}
	in.close();
	out.close();
	return true;
}

StringVec CFileUtilitySTL::split(const tstring &str, const tstring &pattern)
{
	//const char* convert to char*
	TCHAR * strc = new TCHAR[_tcslen(str.c_str()) + 1];
	_tcscpy(strc, str.c_str());
	StringVec resultVec;
	TCHAR* tmpStr = _tcstok(strc, pattern.c_str());
	while (tmpStr != NULL)
	{
		resultVec.push_back(tstring(tmpStr));
		tmpStr = _tcstok(NULL, pattern.c_str());
	}

	delete[] strc;

	return resultVec;
}