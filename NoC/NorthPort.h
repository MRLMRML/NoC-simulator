#pragma once
#include "Port.h"
#include "VirtualChannel.h"

class NorthPort : public Port
{
public:
	NorthPort() { m_portType = PortType::NorthPort; }

	void computeRoute() override;

private:
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};
};