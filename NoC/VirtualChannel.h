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


	//PortType m_portType{PortType::Unselected};
	std::deque<Flit> m_buffer{};
	PortType m_portRouted{PortType::Unselected};
	int m_virtualChannel{-1}; // -1 is idle

	// States
	VirtualChannelState m_virtualChannelState{ VirtualChannelState::I };
	DownstreamVirtualChannelState m_DownstreamVirtualChannelState{ DownstreamVirtualChannelState::I };

	int m_virtualChannelPriority{};
	int m_switchPriority{};
private:
};