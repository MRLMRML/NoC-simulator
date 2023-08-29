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
	int m_virtualChannelPriority{};
	//int m_switchPriority{};


	// Output units
	std::array<VirtualChannelState, VC_NUMBER> m_downstreamVirtualChannelStates{}; // either I or A
	std::array<PortType, VC_NUMBER> m_inputPortRouted{};
	std::array<int, VC_NUMBER> m_inputVirtualChannelAllocated{};
	int m_inputVirtualChannelPriority{};

private:
};