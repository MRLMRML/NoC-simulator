#include "RouterPort.h"

void RouterPort::receiveFlit()
{
	if (!m_inFlitRegister.empty())
	{
		if (m_enable)
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
						log(" Router port: head or headtail flit received ");
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
						log(" Router port: body or tail flit received ");
					}
				}
			}
		}
	}
}

bool RouterPort::receiveCredit()
{
	if (!m_inCreditRegister.empty())
	{
		int credit{ ++m_credit.at(m_inCreditRegister.front().virtualChannel)};
		log(" Router port: credit received ");
		if (credit == BUFFER_SIZE)
			return true; // return true only when credit is refilled completely
	}
	return false;
}