#pragma once
#include "Port.h"
#include "VirtualChannel.h"

class RouterPort : public Port
{
public:
	RouterPort() = default;

	void receiveFlit();

	// Input units
	// Virtual channels
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};

	// Output units
	// Downstream virtual channel states
	std::array<VirtualChannelState, VC_NUMBER> m_downstreamVirtualChannelStates{}; // either I or A
	std::array<PortType, VC_NUMBER> m_portRouted{};
	std::array<int, VC_NUMBER> m_virtualChannelAllocated{};

private:
};