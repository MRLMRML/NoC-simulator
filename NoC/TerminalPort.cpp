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
				m_virtualChannelState = VirtualChannelState::R; // I -> R
				log(" Terminal port: head or headtail flit received ");
			}
		}
		else
		{
			if (m_virtualChannelState == VirtualChannelState::A)
				log(" Terminal port: body or tail flit received ");
		}
	}
}