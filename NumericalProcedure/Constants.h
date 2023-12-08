#pragma once

#include <stdint.h>
#include <string>
#include <format>

namespace constants
{
	const double E_num = 75;
	const double nuD = 100;
	extern uint64_t NUM_OF_CITES;
	extern double E_den;
	extern double nuB;
	extern double nuC;
	extern std::string position_time_origin;
	extern std::string time_reversed;
	extern std::string time_reversed_NB;
	enum class TriangleState { A, B, C, D, E };
}
