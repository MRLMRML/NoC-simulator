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
			m_northPortDownstreamCredits.at(i) = BUFFER_SIZE;
			m_southPortDownstreamCredits.at(i) = BUFFER_SIZE;
			m_westPortDownstreamCredits.at(i) = BUFFER_SIZE;
			m_eastPortDownstreamCredits.at(i) = BUFFER_SIZE;
		}
	}

	void decreaseNorthPortCredits(const int virtualChannelIndex);
	void decreaseSouthPortCredits(const int virtualChannelIndex);
	void decreaseWestPortCredits(const int virtualChannelIndex);
	void decreaseEastPortCredits(const int virtualChannelIndex);

	void increaseNorthPortCredits(const int virtualChannelIndex);
	void increaseSouthPortCredits(const int virtualChannelIndex);
	void increaseWestPortCredits(const int virtualChannelIndex);
	void increaseEastPortCredits(const int virtualChannelIndex);

	int checkNorthPortCredits(const int virtualChannelIndex);
	int checkSouthPortCredits(const int virtualChannelIndex);
	int checkWestPortCredits(const int virtualChannelIndex);
	int checkEastPortCredits(const int virtualChannelIndex);

private:
	std::array<int, VC_NUMBER> m_northPortDownstreamCredits{};
	std::array<int, VC_NUMBER> m_southPortDownstreamCredits{};
	std::array<int, VC_NUMBER> m_westPortDownstreamCredits{};
	std::array<int, VC_NUMBER> m_eastPortDownstreamCredits{};
};