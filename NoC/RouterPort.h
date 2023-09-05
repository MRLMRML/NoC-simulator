#pragma once
#include "Port.h"
#include "VirtualChannel.h"

class RouterPort : public Port
{
public:
	RouterPort() = default;

	void receiveFlit();

	// Input units
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};
	// SA
	int m_virtualChannelSwitched{}; // default 0
	int m_switchPriorityGlobal{};

	// Output units
	// VA
	std::array<VirtualChannelState, VC_NUMBER> m_downstreamVirtualChannelStates{}; // I/V/A
	std::array<PortType, VC_NUMBER> m_inputPortRouted{}; // Unselected (default)
	std::array<int, VC_NUMBER> m_inputVirtualChannelAllocated{}; // default doesn't matter  
	std::array<int, VC_NUMBER> m_inputVirtualChannelPriority{};
	// SA
	std::array<int, VC_NUMBER> m_credit{ BUFFER_SIZE }; // available downstream buffer number
	PortType m_inputPortSwitched{}; // Unselected (default)

private:
};