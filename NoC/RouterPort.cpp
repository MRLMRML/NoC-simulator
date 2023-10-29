#include "RouterPort.h"

void RouterPort::receiveFlit()
{
	if (!m_inFlitRegister.empty())
	{
		if (m_inFlitEnable)
		{
			if (m_inFlitRegister.front().flitType == FlitType::HeadFlit
				|| m_inFlitRegister.front().flitType == FlitType::HeadTailFlit)
			{
				if (m_virtualChannels.at(m_inFlitRegister.front().virtualChannel).m_virtualChannelState == VirtualChannelState::I)
				{
					if (m_virtualChannels.at(m_inFlitRegister.front().virtualChannel).pushbackFlit(m_inFlitRegister.front()))
					{
						m_virtualChannels.at(m_inFlitRegister.front().virtualChannel).m_virtualChannelState = VirtualChannelState::R; // I -> R
						m_inFlitRegister.pop_front();
						logDebug(" Router port: head or headtail flit received ");
					}
				}
			}
			else
			{
				if (m_virtualChannels.at(m_inFlitRegister.front().virtualChannel).m_virtualChannelState == VirtualChannelState::A
					|| m_virtualChannels.at(m_inFlitRegister.front().virtualChannel).m_virtualChannelState == VirtualChannelState::V)
				{
					if (m_virtualChannels.at(m_inFlitRegister.front().virtualChannel).pushbackFlit(m_inFlitRegister.front()))
					{
						m_inFlitRegister.pop_front();
						logDebug(" Router port: body or tail flit received ");
					}
				}
			}
			m_inFlitEnable = false; // receive one flit at a time
		}
	}
}

bool RouterPort::receiveCredit()
{
	if (!m_inCreditRegister.empty())
	{
		if (m_inCreditEnable)
		{
			++m_credit.at(m_inCreditRegister.front().virtualChannel);
			logDebug(" Router port: credit received ");
			return m_inCreditRegister.front().isTailFlit; // return true only when credit is for tail flit
		}
	}
	return false;
}