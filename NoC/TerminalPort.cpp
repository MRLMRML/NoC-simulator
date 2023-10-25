#include "TerminalPort.h"

void TerminalPort::receiveFlit()
{
	if (!m_inFlitRegister.empty())
	{
		if (m_enable)
		{
			if (m_inFlitRegister.front().flitType == FlitType::HeadFlit
				|| m_inFlitRegister.front().flitType == FlitType::HeadTailFlit)
			{
				if (m_virtualChannelState == VirtualChannelState::I)
				{
					m_flitRegister.push_back(m_inFlitRegister.front());
					m_inFlitRegister.pop_front();
					m_virtualChannelState = VirtualChannelState::R; // I -> R
					logDebug(" Terminal port: head or headtail flit received ");
				}
			}
			else
			{
				if (m_virtualChannelState == VirtualChannelState::A
					|| m_virtualChannelState == VirtualChannelState::V)
				{
					m_flitRegister.push_back(m_inFlitRegister.front());
					m_inFlitRegister.pop_front();
					logDebug(" Terminal port: body or tail flit received ");
				}
			}
		}
	}
}