#pragma once
#include "DataStructures.h"
#include "Parameters.h"

class VirtualChannel
{
public:
	VirtualChannel() = default;

	void pushFlit();

	void popFlit();

private:
	std::vector<Flit> m_virtualChannel{};
};