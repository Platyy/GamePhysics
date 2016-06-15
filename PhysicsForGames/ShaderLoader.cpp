#include "ShaderLoader.h"
#include <vector>
#include <fstream>
#include <sstream>
#include "gl_core_4_4.h"

using namespace std;

std::string Resource::ImportShader(const char* _name)
{
	std::string _fullpath = Resource::GetResourcePath() + _ShaderPath + _name + ".glsl";

	std::ifstream _file;
	std::stringstream _stream;

	std::string _contents;

	_file.exceptions(std::ifstream::badbit);
	try
	{
		_file.open(_fullpath);

		if (!_file.good())
		{
			printf("%s '%s'\n", "error: failed to load shader", _name);
			return "";
		}

		_stream << _file.rdbuf();

		_file.close();

		_contents = _stream.str();
	}
	catch (std::ifstream::failure e)
	{
		printf("%s\n", e.what());
		return "";
	}

	return _contents;
}