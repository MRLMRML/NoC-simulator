#include "RouterPort.h"

void RouterPort::receiveFlit()
{
	if (!m_inFlitBuffer.empty())
	{
		m_virtualChannels.at(m_inFlitBuffer.front().virtualChannel).pushbackFlit(m_inFlitBuffer.front());
		m_virtualChannels.at(m_inFlitBuffer.front().virtualChannel).m_virtualChannelState = VirtualChannelState::R; // I -> R
		m_inFlitBuffer.pop_front();
	}
}

void RouterPort::receiveCredit()
{
	if (!m_inCreditBuffer.empty())
	{
		m_credit.at(m_inCreditBuffer.front().virtualChannel)++;
		m_inCreditBuffer.pop_front();
	}
}