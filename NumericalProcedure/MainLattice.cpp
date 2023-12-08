#include "MainLattice.h"
#include "Constants.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include<iomanip>

using namespace constants;

MainLattice::MainLattice() :
_nA{ 0 }, _nB{ NUM_OF_CITES }, _nC{ 0 }, _nD{ 0 }, _nE{ 0 },
_rand{ std::vector<uint64_t>(NUM_OF_CITES) },
_pos{ std::vector<uint64_t>(NUM_OF_CITES) },
_latticeElements{ std::vector<TriangleState>(NUM_OF_CITES) },
_gen(_rd()), _dis(0, 1), _disLattice(0, 1),
_pto{ std::ofstream{position_time_origin} },
_tr{ std::ofstream{time_reversed} },
_trNB{ std::ofstream{time_reversed_NB} },
_currentTime{ -1/(nuB * _nB)}, _reversedNB{0}
{
	_pto << NUM_OF_CITES << '\n';
	_tr << NUM_OF_CITES << '\n';
	_trNB << NUM_OF_CITES << '\n';

	for (uint64_t position = 0; position < NUM_OF_CITES; ++position)
	{
		_latticeElements[position] = TriangleState::B;
		_rand[position] = position;
		_pos[position] = position;
	}
}

double MainLattice::dt() const
{
	return  double(1) / (nuB * _nB + nuC * _nC + nuD * _nD);
}

TriangleState MainLattice::stateToFlip()
{
	double dT = dt();
	double p = _dis(_gen);
	double cProb = 0;
	std::vector<double> probs(3);

	probs[0] = nuB * _nB * dT;
	probs[1] = nuC * _nC * dT;
	probs[2] = nuD * _nD * dT;

	for (int i = 0; i < probs.size(); ++i)
	{
		cProb += probs[i];
		if (p <= cProb)
			return static_cast<TriangleState>(i + 1);
	}

	return TriangleState::A;
}

uint64_t MainLattice::nX(TriangleState state) const
{
	switch (static_cast<int>(state))
	{
	case 0:
		return _nA;
	case 1:
		return _nB;
	case 2:
		return _nC;
	case 3:
		return _nD;
	case 4:
		return _nE;
	default:
		throw "wtf with nX?\n";
		break;
	}
}

uint64_t MainLattice::elementFromStateToFlip(TriangleState state)
{
	uint64_t N = nX(state);

	if (state == TriangleState::A)
		throw "Can not flip triangle with state A\n";

	if (N == 0)
		throw "There are no elements to flip\n";

	_disLattice = std::uniform_int_distribution<uint64_t>(0, N-1);

	return _disLattice(_gen);
}

void MainLattice::decreaseNx(TriangleState state)
{
	switch (static_cast<int>(state))
	{
	case 0:
		throw "Do you want to decrease element from A?\n";
		break;
	case 1: 
		_nB -= 1;
		break;
	case 2:
		_nC -= 1;
		break;
	case 3:
		_nD -= 1;
		break;
	case 4:
		_nE -= 1;
		break;
	default:
		throw "DecreaseNx works incorrectly!\n";
		break;
	}

	return;
}

void MainLattice::increaseNx(TriangleState state)
{
	switch (static_cast<int>(state))
	{
	case 0:
		_nA += 1;
		break;
	case 1:
		throw "Do you want to increase element from B?\n";
		break;
	case 2:
		_nC += 1;
		break;
	case 3:
		_nD += 1;
		break;
	case 4:
		_nE += 1;
		break;
	default:
		throw "IncreaseNx works incorrectly!\n";
		break;
	}

	return;
}

void MainLattice::swap(uint64_t firstInd, uint64_t secondInd)
{
	if (firstInd != secondInd)
	{
		std::swap(_pos[_rand[firstInd]], _pos[_rand[secondInd]]);
		std::swap(_rand[firstInd], _rand[secondInd]);
	}

	return;
}

uint64_t MainLattice::putElementToAnotherClass(TriangleState initialState, uint64_t randomElement, 
																	TriangleState finalState)
{
	int init =	static_cast<int>(initialState);
	int final =	static_cast<int>(finalState);

	if (init == 2 && final == 0)
		int a = 1;

	uint64_t posInRand = randomElement;

	for (int i = 0; i < init; ++i)
		posInRand += nX(static_cast<TriangleState>(static_cast<int>(i)));

	uint64_t posInLattice = _rand[posInRand];

	if (finalState == TriangleState::A || finalState == TriangleState::E)
	{
		if(finalState == TriangleState::A)
			setTime(_currentTime + dt());

		_pto << posInLattice << " " << std::setprecision(10) << _currentTime << " "
			<< (initialState == TriangleState::B) << '\n';
		_tr <<  std::setprecision(10) << _currentTime << " " << _nA + _nE << '\n';

		if (initialState == TriangleState::B)
		{
			_reversedNB++;
			_trNB << std::setprecision(10) <<_currentTime << " " << _reversedNB << '\n';
		}
	}

	uint64_t lastPos = -1;
	uint64_t firstPos;

	for (int i = 0; i <= init; ++i)
		lastPos += nX(static_cast<TriangleState>(static_cast<int>(i)));

	firstPos = lastPos - nX(static_cast<TriangleState>(static_cast<int>(init))) + 1;

	if (init < final)
	{
		for (int i = init; i < final; ++i)
		{
			swap(posInRand, lastPos);
			posInRand = lastPos;
			lastPos += nX(static_cast<TriangleState>(static_cast<int>(i)));
		}
	}
	else if (init == final)
	{
		throw "INIT == FINAL\n";
	}
	else
	{
		for (int i = init-1; i >= final; --i) 
		{
			swap(posInRand, firstPos);
			posInRand = firstPos;
			firstPos -= nX(static_cast<TriangleState>(static_cast<int>(i)));
		}
	}

	decreaseNx(initialState);
	increaseNx(finalState);
	_latticeElements[posInLattice] = finalState;

	return posInLattice;
}

std::vector<uint64_t> MainLattice::defineNeighbors(uint64_t posOnLattice) const
{
	std::vector<uint64_t> neighbors(3, 0);
	uint64_t n = sqrt(NUM_OF_CITES);
	uint64_t x, y;

	y = posOnLattice / n;
	x = posOnLattice % n;

	//if (x == 0)
	//	neighbors[0] = y * n + n - 1;
	//else
	//	neighbors[0] = posOnLattice - 1;

	//if (x == n - 1)
	//	neighbors[1] = y * n;
	//else
	//	neighbors[1] = posOnLattice + 1;

	//if (posOnLattice % 2 == 0)
	//{
	//	if (y == n - 1)
	//		neighbors[2] = posOnLattice % n;
	//	else
	//		neighbors[2] = posOnLattice + n;
	//}
	//else
	//{
	//	if (y == 0)
	//		neighbors[2] = NUM_OF_CITES - n + posOnLattice;
	//	else
	//		neighbors[2] = posOnLattice - n;
	//}
	neighbors[0] = n * y + (x + 1) % n;
	neighbors[1] = n * y + (x - 1) * (x > 0);

	if (y % 2 == 0) // first triangle is up
	{
		if (x % 2 == 0)
			neighbors[2] = n * ((y + 1) % n) + x;
		else
			neighbors[2] = n * ((y - 1) * (y > 0)) + x;
	}
	else // first triangle is down
	{
		if (x % 2 == 0)
			neighbors[2] = n * ((y - 1) * (y > 0)) + x;
		else
			neighbors[2] = n * ((y + 1) % n) + x;
	}

	return neighbors;
}

void MainLattice::flipRandomTriangle()
{
	TriangleState initState = stateToFlip();
	uint64_t randElement = elementFromStateToFlip(initState);
	uint64_t randElementLatticePos = putElementToAnotherClass(initState, randElement, TriangleState::A);
	std::vector<uint64_t> neighbors = defineNeighbors(randElementLatticePos);

	for (auto element : neighbors)
	{
		TriangleState initState = _latticeElements[element];
		TriangleState finalState;

		uint64_t elemInRand = _pos[element];
		for (int i = 0; i < static_cast<int>(initState); ++i)
			elemInRand -= nX(static_cast<TriangleState>(static_cast<int>(i)));

		if (initState == TriangleState::A)
			continue;

		if (initState != TriangleState::E)
			finalState = static_cast<TriangleState> (static_cast<int>(initState) + 1);
		else
			finalState = TriangleState::A;

		putElementToAnotherClass(initState, elemInRand, finalState);
	}
}

void MainLattice::printLattice() const
{
	uint64_t n = sqrt(NUM_OF_CITES);

	for (uint64_t i = 0; i < NUM_OF_CITES; ++i)
	{
		std::cout << std::setw(2) << static_cast<int>(_latticeElements[i]);

		if (i % n == n - 1)
			std::cout << std::endl;
	}

	std::cout << "---------------------------------------------------------- \n";
}

void MainLattice::printVector(const std::vector<uint64_t>& data) const
{
	for (const auto& x : data)
		std::cout << x << " ";
	std::cout << "\n";
}