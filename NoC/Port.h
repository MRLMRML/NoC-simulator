#pragma once
#include "DataStructures.h"
#include "VirtualChannel.h"
#include "CreditMaintainer.h"

class Port
{
public:
	// Input port
	std::deque<Flit> outFlitBuffer;
	std::deque<Credit> inCreditBuffer;

	// Output port
	std::deque<Flit> inFlitBuffer;
	std::deque<Credit> outCreditBuffer;

	bool areEmpty();

private:
	std::array<VirtualChannel, VC_NUMBER> m_virtualChannels{};
	CreditMaintainer m_creditMaintainer{};
};