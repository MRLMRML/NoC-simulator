#include "CreditMaintainer.h"

void CreditMaintainer::decreaseCredit(const int virtualChannelIndex)
{
	m_creditMaintainer.at(virtualChannelIndex)--;
}

void CreditMaintainer::increaseCredit(const int virtualChannelIndex)
{
	m_creditMaintainer.at(virtualChannelIndex)++;
}

int CreditMaintainer::checkCredit(const int virtualChannelIndex)
{
	return m_creditMaintainer.at(virtualChannelIndex);
}