#pragma once

#include <stdint.h>
#include <random>
#include <iterator>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>

#include "MainLattice.h"

using namespace constants;

void fastNumProcess()
{
	MainLattice lattice;

	while (lattice.nX(TriangleState::A) + lattice.nX(TriangleState::E) < NUM_OF_CITES)
	{
		lattice.flipRandomTriangle();
	}
}
