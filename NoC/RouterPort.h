#pragma once
#include "Port.h"
#include "VirtualChannel.h"
#include "CreditMaintainer.h"

class RouterPort : public Port
{
public:
	RouterPort() = default;

	void runOneStep() override;

private:
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};
	CreditMaintainer m_creditMaintainer{};
};