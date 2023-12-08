#pragma once

#include <vector>
#include <string>

class ConfigurationReader
{
public:
	ConfigurationReader(std::string path, double time);
	void latticeToPixelmap(int width);
	std::vector<std::string> split(std::string s, std::string delimiter);
	std::vector<float>  _pixelMap;
	std::vector<bool> _configuration;
	double _time;
};
