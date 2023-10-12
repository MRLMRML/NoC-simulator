#pragma once
#include "DataStructures.h"
#include "Parameters.h"
#include "Clock.h"

class VirtualChannel
{
public:
	VirtualChannel() = default;

	bool pushbackFlit(Flit flit);

	Flit popfrontFlit();

	bool isEmpty();

	//void runOneStep();

	// Data
	std::deque<Flit> m_buffer{};

	// Control
	VirtualChannelState m_virtualChannelState{}; // I(default) -> R -> V -> A
	PortType m_outputPortRouted{}; // Unselected (default)
	int m_outputVirtualChannelAllocated{-1}; // -1 is default
	// VA
	int m_virtualChannelPriority{};
	// SA
	int m_switchPriorityLocal{};

	// Timing
	Clock m_localClock{};
private:
};