#pragma once
#include "DataStructures.h"

struct Port
{
	std::deque<Flit> outBuffer;
	std::deque<Flit> inBuffer;
	bool isEmpty();
};