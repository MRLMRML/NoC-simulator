#pragma once
#include "DataStructures.h"
#include "Parameters.h"

class VirtualChannel
{
public:
	VirtualChannel() = default;

	void pushbackFlit(Flit flit);

	void popfrontFlit();


private:
	std::deque<Flit> m_buffer{};
	Direction m_port{Direction::X}; // X is idle
	int m_virtualChannel{-1}; // -1 is idle
};