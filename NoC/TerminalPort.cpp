#include "TerminalPort.h"

void TerminalPort::receiveFlit()
{
	if (m_inFlitRegister.valid == true)
	{
		if (m_inFlitRegister.flit.flitType == FlitType::HeadFlit
			|| m_inFlitRegister.flit.flitType == FlitType::HeadTailFlit)
		{
			if (m_virtualChannelState != VirtualChannelState::I)
			{
				throw std::runtime_error{ " Terminal port: virtual channel is not in state I " };
			}
			m_virtualChannelState = VirtualChannelState::R; // I -> R
		}
	}
}