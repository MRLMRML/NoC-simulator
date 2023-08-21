#pragma once
#include "Port.h"
#include "VirtualChannel.h"

class WestPort : public Port
{
public:
	WestPort() { m_portType = PortType::WestPort; }

	void computeRoute() override;

private:
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};
};