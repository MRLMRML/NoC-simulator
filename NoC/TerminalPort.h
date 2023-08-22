#pragma once
#include "Port.h"

class TerminalPort : public Port
{
public:
	TerminalPort() = default;

public:
	//PortType m_portType{PortType::Unselected};
	//std::deque<Flit> m_buffer{};
	PortType m_portRouted{ PortType::Unselected };
	//int m_virtualChannel{ -1 }; // -1 is idle

	VirtualChannelState m_virtualChannelState{ VirtualChannelState::R };

	int m_virtualChannelPriority{}; // not real VC in terminal port
	int m_switchPriority{};
private:
};