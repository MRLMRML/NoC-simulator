#include "VirtualChannel.h"

void VirtualChannel::pushbackFlit(Flit flit)
{
	if (m_buffer.size() == BUFFER_SIZE)
		throw std::runtime_error{ " Virtual channel error: buffer full " };
	
	m_buffer.push_back(flit);

	//// set BodyFlit/TailFlit by VC fields
	//if (flit.flitType == FlitType::BodyFlit 
	//	|| flit.flitType == FlitType::TailFlit)
	//{
	//	flit.port = m_portRouted;
	//	flit.virtualChannel = m_virtualChannel;
	//}
}

void VirtualChannel::popfrontFlit()
{
	//if (m_buffer.front().flitType == FlitType::HeadTailFlit 
	//	|| m_buffer.front().flitType == FlitType::TailFlit)
	//{
	//	m_port = PortType::X;
	//	m_virtualChannel = -1;
	//}

	m_buffer.pop_front();
}
