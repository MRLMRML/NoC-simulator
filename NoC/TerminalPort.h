#pragma once
#include "Port.h"

class TerminalPort : public Port
{
public:
	TerminalPort() = default;

public:
	// States
	VirtualChannelState m_virtualChannelState{}; // I(default) -> R -> V -> A

	// Fields
	PortType m_portRouted{}; // Unselected (default)
	int m_virtualChannelAllocated{ -1 }; // -1 is default

	// Priorities
	int m_virtualChannelPriority{}; // not real VC in terminal port
	int m_switchPriority{};

	// Downstream virtual channel states, needed in terminal port
	std::array<VirtualChannelState, VC_NUMBER> m_DownstreamVirtualChannelStates{}; // either I or A

private:
};