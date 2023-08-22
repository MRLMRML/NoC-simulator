#pragma once
#include "Port.h"
#include "VirtualChannel.h"

class RouterPort : public Port
{
public:
	RouterPort() = default;
	//RouterPort(const PortType portType) : Port{ portType } {}

	void receiveFlit();

	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};
private:
};