#pragma once
#include "DataStructures.h"

class Port
{
public:
	Port() = default;
	virtual ~Port() = default;

	// Input port
	std::deque<Flit> outFlitBuffer;
	std::deque<Credit> inCreditBuffer;

	// Output port
	std::deque<Flit> inFlitBuffer;
	std::deque<Credit> outCreditBuffer;

	bool isEmpty();

	Direction computeRoute(const Direction port, Flit flit);

	virtual void runOneStep() {}

private:
};