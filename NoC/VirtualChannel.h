#pragma once
#include "DataStructures.h"
#include "Parameters.h"

class VirtualChannel
{
public:
	VirtualChannel() = default;


	void pushbackFlit(Flit flit);

	void popfrontFlit();

	void runOneStep();


	std::deque<Flit> m_buffer{};

	// States
	VirtualChannelState m_virtualChannelState{}; // I(default) -> R -> V -> A

	// Fields
	PortType m_portRouted{}; // Unselected (default)
	int m_virtualChannelAllocated{-1}; // -1 is default

	// Priorities
	int m_virtualChannelPriority{};
	int m_switchPriority{};

private:
};