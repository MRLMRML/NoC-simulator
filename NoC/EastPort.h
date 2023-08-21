#pragma once
#include "Port.h"
#include "VirtualChannel.h"

class EastPort : public Port
{
public:
	EastPort() { m_portType = PortType::EastPort; }

	void computeRoute() override;

private:
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};
};