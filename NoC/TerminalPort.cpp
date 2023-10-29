#include "TerminalPort.h"

void TerminalPort::receiveFlit()
{
	if (!m_inFlitRegister.empty())
	{
		if (m_inFlitEnable)
		{
			if (m_inFlitRegister.front().flitType == FlitType::HeadFlit
				|| m_inFlitRegister.front().flitType == FlitType::HeadTailFlit)
			{
				if (m_virtualChannel.m_virtualChannelState == VirtualChannelState::I)
				{
					if (m_virtualChannel.pushbackFlit(m_inFlitRegister.front()))
					{
						m_virtualChannel.m_virtualChannelState = VirtualChannelState::R; // I -> R
						m_inFlitRegister.pop_front();
						logDebug(" Terminal port: head or headtail flit received ");
					}
				}
			}
			else
			{
				if (m_virtualChannel.m_virtualChannelState == VirtualChannelState::A
					|| m_virtualChannel.m_virtualChannelState == VirtualChannelState::V)
				{
					if (m_virtualChannel.pushbackFlit(m_inFlitRegister.front()))
					{
						m_inFlitRegister.pop_front();
						logDebug(" Terminal port: body or tail flit received ");
					}
				}
			}
			m_inFlitEnable = false; // receive one flit at a time
		}
	}
}