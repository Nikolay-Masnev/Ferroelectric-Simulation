#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

#include "Triangle.h"
#include "Constants.h"

using namespace constants;

class MainLattice
{
public:
	MainLattice();

	double dt() const;
	TriangleState stateToFlip();
	uint64_t elementFromStateToFlip(TriangleState state);
	uint64_t nX(TriangleState state) const;
	void decreaseNx(TriangleState state);
	void increaseNx(TriangleState state);
	uint64_t putElementToAnotherClass(TriangleState initState, uint64_t randomElement, TriangleState finalState);
	void swap(uint64_t firstInd, uint64_t secondInd);
	std::vector<uint64_t> defineNeighbors(uint64_t posOnLattice) const;
	void flipRandomTriangle();
	void printLattice() const;
	void printVector(const std::vector<uint64_t>& data) const;
	void setTime(double time) { _currentTime = time; }
private:
	uint64_t _nA, _nB, _nC, _nD, _nE;
	std::vector<uint64_t> _rand;
	std::vector<uint64_t> _pos;
	std::vector<TriangleState> _latticeElements;

	std::random_device _rd;
	std::mt19937 _gen;
	std::uniform_real_distribution<> _dis;
	std::uniform_int_distribution<uint64_t> _disLattice;

	std::ofstream _pto;
	std::ofstream _tr;
	std::ofstream _trNB;

	uint64_t _reversedNB;
	double _currentTime;
};
