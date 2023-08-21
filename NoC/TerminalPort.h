#pragma once
#include "Port.h"

class TerminalPort : public Port
{
public:
	TerminalPort() { m_portType = PortType::TerminalPort; }

	void computeRoute() override;

public:
	//PortType m_portType{PortType::Unselected};
	std::deque<Flit> m_buffer{};
	PortType m_portRouted{ PortType::Unselected };
	//int m_virtualChannel{ -1 }; // -1 is idle

	int m_virtualChannelPriority{}; // not real VC in terminal port
	int m_switchPriority{};
private:
};