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

	// Output units
	std::array<VirtualChannelState, VC_NUMBER> m_downstreamVirtualChannelStates{}; // either I or A
	std::array<PortType, VC_NUMBER> m_inputPortRouted{};
	std::array<int, VC_NUMBER> m_inputVirtualChannelAllocated{};
	int m_inputVirtualChannelPriority{};

private:
};