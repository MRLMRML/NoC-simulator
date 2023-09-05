#pragma once
#include "Port.h"

class TerminalPort : public Port
{
public:
	TerminalPort() = default;

public:

	// Input units
	VirtualChannelState m_virtualChannelState{}; // I(default) -> R -> V -> A
	PortType m_outputPortRouted{}; // Unselected (default)
	int m_outputVirtualChannelAllocated{ -1 }; // -1 is default
	// VA
	int m_virtualChannelPriority{};
	// SA
	int m_switchPriorityGlobal{};

	// Output units
	// VA
	VirtualChannelState m_downstreamVirtualChannelState{}; // I/V/A
	PortType m_inputPortRouted{};
	int m_inputVirtualChannelAllocated{};
	int m_inputVirtualChannelPriority{};
	// SA
	PortType m_inputPortSwitched{}; // Unselected (default)
private:
};