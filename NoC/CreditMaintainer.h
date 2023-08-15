#pragma once
#include "DataStructures.h"
#include "Parameters.h"

class CreditMaintainer
{
public:
	CreditMaintainer()
	{
		for (int i{}; i < VC_NUMBER; ++i)
		{
			m_creditMaintainer.at(i) = BUFFER_SIZE;
		}
	}

	void decreaseCredit(const int virtualChannelIndex);
	void increaseCredit(const int virtualChannelIndex);
	int checkCredit(const int virtualChannelIndex);

private:
	std::array<int, VC_NUMBER> m_creditMaintainer{};
};