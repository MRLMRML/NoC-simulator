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

	// Data
	std::deque<Flit> m_buffer{};

	// Control
	VirtualChannelState m_virtualChannelState{}; // I(default) -> R -> V -> A
	PortType m_outputPortRouted{}; // Unselected (default)
	int m_outputVirtualChannelAllocated{-1}; // -1 is default
	int m_virtualChannelPriority{};
	//int m_switchPriority{};

private:
};