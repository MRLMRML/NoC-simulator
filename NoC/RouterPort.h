#pragma once
#include "Port.h"
#include "VirtualChannel.h"

class RouterPort : public Port
{
public:
	RouterPort() = default;
	RouterPort(const PortType portType) : Port{ portType } 
	{
		m_credit.fill(BUFFER_SIZE);
	}

	void receiveFlit() override;
	bool receiveCredit();

	// Input units
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};

	// Output units
	// VA
	std::array<VirtualChannelState, VC_NUMBER> m_downstreamVirtualChannelStates{}; // I/V/A
	std::array<PortType, VC_NUMBER> m_inputPortRouted{}; // Unselected (default)
	std::array<int, VC_NUMBER> m_inputVirtualChannelAllocated{}; // default doesn't matter  
	std::array<int, VC_NUMBER> m_inputVirtualChannelPriority{};
	// SA
	std::array<int, VC_NUMBER> m_credit{}; // available downstream buffer number

private:
};