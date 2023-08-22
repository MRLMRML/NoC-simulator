#include "CreditMaintainer.h"

void CreditMaintainer::decreaseNorthPortCredits(const int virtualChannelIndex)
{
	m_northPortDownstreamCredits.at(virtualChannelIndex)--;
}

void CreditMaintainer::decreaseSouthPortCredits(const int virtualChannelIndex)
{
	m_southPortDownstreamCredits.at(virtualChannelIndex)--;
}

void CreditMaintainer::decreaseWestPortCredits(const int virtualChannelIndex)
{
	m_westPortDownstreamCredits.at(virtualChannelIndex)--;
}

void CreditMaintainer::decreaseEastPortCredits(const int virtualChannelIndex)
{
	m_eastPortDownstreamCredits.at(virtualChannelIndex)--;
}

void CreditMaintainer::increaseNorthPortCredits(const int virtualChannelIndex)
{
	m_northPortDownstreamCredits.at(virtualChannelIndex)++;
}

void CreditMaintainer::increaseSouthPortCredits(const int virtualChannelIndex)
{
	m_southPortDownstreamCredits.at(virtualChannelIndex)++;
}

void CreditMaintainer::increaseWestPortCredits(const int virtualChannelIndex)
{
	m_westPortDownstreamCredits.at(virtualChannelIndex)++;
}

void CreditMaintainer::increaseEastPortCredits(const int virtualChannelIndex)
{
	m_eastPortDownstreamCredits.at(virtualChannelIndex)++;
}

int CreditMaintainer::checkNorthPortCredits(const int virtualChannelIndex)
{
	return m_northPortDownstreamCredits.at(virtualChannelIndex);
}

int CreditMaintainer::checkSouthPortCredits(const int virtualChannelIndex)
{
	return m_southPortDownstreamCredits.at(virtualChannelIndex);
}

int CreditMaintainer::checkWestPortCredits(const int virtualChannelIndex)
{
	return m_westPortDownstreamCredits.at(virtualChannelIndex);
}

int CreditMaintainer::checkEastPortCredits(const int virtualChannelIndex)
{
	return m_eastPortDownstreamCredits.at(virtualChannelIndex);
}