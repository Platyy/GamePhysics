#pragma once
#include <stdio.h>

#ifdef _WIN32

#include <direct.h>
#define GetCurrentDir _getcwd

#else

#include <unistd.h>
#define GetCurrentDir getcwd

#endif

#include <string>
#include <vector>
#include <map>

#define _ResourcePath ("")
#define _ShaderPath ("shaders\\")

class Resource
{
public:
	static std::string GetWorkingDirectory()
	{
		char _path[FILENAME_MAX];

		if (!GetCurrentDir(_path, sizeof(_path)))
			return "";

		return std::string(_path) + "\\";
	}

	static std::string GetResourcePath()
	{
		return GetWorkingDirectory() + _ResourcePath;
	}

	static std::string ImportShader(const char* _name);

};