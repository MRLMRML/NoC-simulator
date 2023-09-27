#include "RouterPort.h"

void RouterPort::receiveFlit()
{
	if (!m_inFlitRegister.empty())
	{
		m_virtualChannels.at(m_inFlitRegister.front().virtualChannel).pushbackFlit(m_inFlitRegister.front());
		m_virtualChannels.at(m_inFlitRegister.front().virtualChannel).m_virtualChannelState = VirtualChannelState::R; // I -> R
		m_inFlitRegister.pop_front();
	}
}

void RouterPort::receiveCredit()
{
	if (!m_inCreditRegister.empty())
	{
		m_credit.at(m_inCreditRegister.front().virtualChannel)++;
		m_inCreditRegister.pop_front();
	}
}