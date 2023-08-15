#pragma once
#include "DataStructures.h"
#include "Parameters.h"

class VirtualChannel
{
public:
	VirtualChannel() = default;

	void pushbackFlit(const Flit flit);

	void popfrontFlit();


private:
	std::deque<Flit> m_virtualChannel{};
};