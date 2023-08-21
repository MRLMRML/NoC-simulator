#pragma once
#include "Port.h"
#include "VirtualChannel.h"

class SouthPort : public Port
{
public:
	SouthPort() { m_portType = PortType::SouthPort; }

	void receiveFlit();

	void computeRoute() override;

private:
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};
};
