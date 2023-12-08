#include "Constants.h"
#include "NumProcess.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

using namespace constants;

int main(int argc, char* argv[])
{
	if (argc == 3)
	{
		std::string value = argv[1];
		char* end;
		NUM_OF_CITES = strtoull(value.c_str(), &end, 10);

		value = argv[2];
		E_den = std::stod(value);

		nuB = nuD * exp(-E_num / E_den);
		nuC = nuD * exp(-0.19 * E_num / E_den);
		position_time_origin = "../data/position_time_origin_" +
			std::format("{:.2f}", E_den) + "kV_" + std::to_string(NUM_OF_CITES) + ".txt";
		time_reversed = "../data/time_reversed" +
			std::format("{:.2f}", E_den) + "kV_" + std::to_string(NUM_OF_CITES) + ".txt";
		time_reversed_NB = "../data/time_reversed_NB" +
			std::format("{:.2f}", E_den) + "kV_" + std::to_string(NUM_OF_CITES) + ".txt";
	}
	else
		std::cout << "Incorrect input\n";

	auto start = std::chrono::high_resolution_clock::now();
	fastNumProcess();
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast
		<std::chrono::microseconds>(stop - start);
	std::cout << "Time taken by function: " << 
		duration.count() / 1e6 << " seconds" << std::endl;

	return 0;
}