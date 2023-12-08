#include "ConfigurationReader.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

ConfigurationReader::ConfigurationReader(std::string path, double time) : _time(0.)
{
	std::ifstream file;
	file.open(path);

	if (file.is_open())
	{
		std::string data;
		std::string::size_type sz = 0;
		int64_t counter = 0;
		uint64_t N = 0;
		double tmpTime;
		uint64_t tmpPos;
		std::string delimiter = " ";
		std::vector<std::string> splittedData;

		while (getline(file, data) && _time < time) 
		{
			if (counter == 0)
			{
				N = std::stoll(data, &sz);
				_configuration = std::vector<bool>(N, false);
				counter++;
				continue;
			}
			
			splittedData = split(data, delimiter);

			tmpPos = std::stoll(splittedData[0], &sz);
			tmpTime = std::stod(splittedData[1], &sz);

			_time = tmpTime;
			_configuration[tmpPos] = true;

			counter++;
		}

		std::cout << counter << std::endl;

		file.close();
	}
}

void ConfigurationReader::latticeToPixelmap(int width)
{
	_pixelMap = std::vector<float>(width * width);

	uint64_t M = sqrt(_pixelMap.size());
	uint64_t N = _configuration.size();
	uint64_t n = sqrt(N);

	for (uint64_t pos = 0; pos < _pixelMap.size(); ++pos)
	{
		uint64_t row = pos / M;
		uint64_t col = pos % M;

		float average = 0.;

		for (uint64_t i = (n / M) * row; i < (n / M) * (row + 1); ++i)
			for (uint64_t j = (n / M) * col; j < (n / M) * (col + 1); ++j)
				average += _configuration[i * n + j];

		average /= ((n / M) * (n / M));

		_pixelMap[pos] = average;
	}
}

std::vector<std::string> ConfigurationReader::split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}