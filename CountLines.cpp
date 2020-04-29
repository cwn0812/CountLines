// CreateAndProject.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <io.h>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <Windows.h>
#include <mutex>
#include <iostream>  
#include <fstream>  
#include <string.h>  
#include <stdlib.h>

using namespace std;

std::string getRuntimePath()
{
	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	fname[_MAX_FNAME];
	TCHAR	ext[_MAX_EXT];
	TCHAR	szFileDir[_MAX_EXT];

	_tsplitpath(szFilePath, drive, dir, fname, ext);
	wsprintf(szFileDir, _T("%s%s"), drive, dir);

	return szFileDir;
}

static std::string settingFilePath = "";

struct RuleData
{
	bool bCountNullLine;

	RuleData(){
		memset(this, 0, sizeof(*this));
	}
};

std::string getConfigValue(const std::string& key, std::string defaultValue="")
{
	char temp[200] = { 0 };
	GetPrivateProfileString("main", key.c_str(), "", temp, sizeof(temp), settingFilePath.c_str());
	std::string result = temp;
	if (strcmp(temp, "") == 0)
	{
		return defaultValue;
	}
	return temp;
}

string getFileExt(const std::string& filePath){
	auto pos = filePath.rfind(".");
	const int& nLen = filePath.length();
	if (pos >= 0 && pos < nLen)
	{
		return filePath.substr(pos + 1, nLen);
	}
	return "";
}

int getFileLine(string filePath, const RuleData& ruleData){
	int nLines = 0;
	ifstream ReadFile;
	int n = 0;
	string tmp;
	ReadFile.open(filePath, ios::in);//ios::in 表示以只读的方式读取文件
	if (ReadFile.fail())//文件打开失败:返回0
		return 0;
	else//文件存在
	{
		while (getline(ReadFile, tmp))
		{
			if (!ruleData.bCountNullLine && tmp == "")
				continue;	//不统计空行

			n++;
		}
	}
	ReadFile.close();
	return n;
}

int countDirTotalLines(const string& dirPath, string secDirPath, const std::map<string, bool>& mapFileExt, const RuleData& ruleData){
	int nTotalLines = 0;

	long hFile = 0;
	struct _finddata_t fileInfo;
	std::string pathName, exdName;
	string folderPath = dirPath + secDirPath;
	if ((hFile = _findfirst(pathName.assign(folderPath).append("\\*").c_str(), &fileInfo)) == -1) {
		return nTotalLines;
	}
	do {
		const std::string filename = fileInfo.name;
		if (filename == "." || filename == "..")
			continue;

		string secDirPath2 = secDirPath + "/" + filename;
		if (fileInfo.attrib&_A_SUBDIR)	//文件夹-
		{
			int nLines = countDirTotalLines(dirPath, secDirPath2, mapFileExt, ruleData);
			nTotalLines += nLines;
			printf("%s总行数：%d\n", secDirPath2.c_str(), nLines);
		}
		else{
			const std::string& filepath = folderPath + "/" + filename;
			string fileExt = getFileExt(filename);
			if (mapFileExt.count(fileExt) > 0)
			{
				int nLines = getFileLine(filepath, ruleData);
				nTotalLines += nLines;
				printf("%s/%s行数：%d\n", secDirPath2.c_str(), filename.c_str(), nLines);
			}
		}

	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return nTotalLines;
}

bool writeFileData(const std::string& filepath, const std::string& filedata)
{
	auto pFile = fopen(filepath.c_str(), "wb+");
	if (!pFile)
		return false;

	for (int i = 0; i < filedata.length(); ++i)
	{
		fputc(filedata[i], pFile);
	}

	fclose(pFile);
	return true;
}

std::vector<std::string> split(std::string str, std::string fengefu)
{
	std::vector<std::string> vec;
	auto pos = str.find(fengefu);
	if (pos >= 0 && pos < str.length())
	{
		std::string result = str.substr(0, pos);
		vec.push_back(result);
		str = str.substr(pos + 1, str.length());
		pos = str.find(fengefu);
	}
	if (str != "")
	{
		vec.push_back(str);
	}
	return vec;
}


int _tmain(int argc, _TCHAR* argv[])
{
	std::string runtimePath = getRuntimePath();
	settingFilePath = runtimePath + "/setting.ini";

	std::thread([=](){
		string dicPath = getConfigValue("dicPath");
		string fileExt = getConfigValue("fileExt");
		RuleData ruleData;
		ruleData.bCountNullLine = getConfigValue("bCountNullLine", "1") == "1";

		std::vector<string> vecFileExt = split(fileExt, ",");
		std::map<string, bool> mapFileExt;
		for (int i = 0; i < vecFileExt.size(); ++i)
		{
			mapFileExt[vecFileExt[i]] = true;
		}

		int nLines = countDirTotalLines(dicPath, "", mapFileExt, ruleData);
		string output = runtimePath + "/output.txt";
		printf("文件总行数: %d\n", nLines);
		char chTemp[100] = { 0 };
		sprintf(chTemp, "total file len:%d", nLines);
		string filedata = chTemp;
		writeFileData(output, filedata);
	}).detach();

	int c;
	printf("输入‘q’退出！\n");
	while (1)
	{
		if (c = getchar() == 'q')
		{
			return 0;             //只有输入A才会返回、退出
		}
	}

	return 0;
}

