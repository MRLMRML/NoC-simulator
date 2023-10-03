#include "RouterPort.h"

void RouterPort::receiveFlit()
{
	if (m_inFlitRegister.valid == true)
	{
		if (m_inFlitRegister.flit.flitType == FlitType::HeadFlit
			|| m_inFlitRegister.flit.flitType == FlitType::HeadTailFlit)
		{
			if (m_virtualChannels.at(m_inFlitRegister.flit.virtualChannel).m_virtualChannelState != VirtualChannelState::I)
			{
				throw std::runtime_error{ " Router port: virtual channel is not in state I " };
			}
			m_virtualChannels.at(m_inFlitRegister.flit.virtualChannel).m_virtualChannelState = VirtualChannelState::R; // I -> R
		}
		
		if (m_virtualChannels.at(m_inFlitRegister.flit.virtualChannel).pushbackFlit(m_inFlitRegister.flit) == true)
		{
			m_inFlitRegister.valid = false;
			log(" Router port: flit received ");
		}
	}
}

bool RouterPort::receiveCredit()
{
	if (m_inCreditRegister.valid == true)
	{
		m_credit.at(m_inCreditRegister.credit.virtualChannel)++;
		m_inCreditRegister.valid = false;
		log(" Router port: credit received ");
		if (m_credit.at(m_inCreditRegister.credit.virtualChannel) == BUFFER_SIZE)
			return true; // return true only when credit is refilled completely
	}
	return false;
}