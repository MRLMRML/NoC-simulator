#pragma once
#include "Port.h"

class TerminalPort : public Port
{
public:
	TerminalPort() = default;
	TerminalPort(const PortType portType) : Port{ portType } {}

	void receiveFlit() override;

	// Input units
	VirtualChannelState m_virtualChannelState{}; // I(default) -> R -> V -> A
	PortType m_outputPortRouted{}; // Unselected (default)
	int m_outputVirtualChannelAllocated{ -1 }; // -1 is default
	// VA
	int m_virtualChannelPriority{};

	// Output units
	// VA
	VirtualChannelState m_downstreamVirtualChannelState{}; // I/V/A
	PortType m_inputPortRouted{};
	int m_inputVirtualChannelAllocated{};
	int m_inputVirtualChannelPriority{};

private:
};