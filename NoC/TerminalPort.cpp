#include "TerminalPort.h"

void TerminalPort::receiveFlit()
{
	if (m_inFlitRegister.valid == true)
	{
		if (m_inFlitRegister.flit.flitType == FlitType::HeadFlit
			|| m_inFlitRegister.flit.flitType == FlitType::HeadTailFlit)
		{
			if (m_virtualChannelState == VirtualChannelState::I)
			{
				if (m_flitRegister.valid == false)
				{
					m_flitRegister.flit = m_inFlitRegister.flit;
					m_inFlitRegister.valid = false;
					m_flitRegister.valid = true;
					m_virtualChannelState = VirtualChannelState::R; // I -> R
					log(" Terminal port: head or headtail flit received ");
				}
			}
		}
		else
		{
			if (m_virtualChannelState == VirtualChannelState::A)
			{
				if (m_flitRegister.valid == false)
				{
					m_flitRegister.flit = m_inFlitRegister.flit;
					m_inFlitRegister.valid = false;
					m_flitRegister.valid = true;
					log(" Terminal port: body or tail flit received ");
				}
			}
		}
	}
}