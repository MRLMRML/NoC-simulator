#include "VirtualChannel.h"

bool VirtualChannel::pushbackFlit(Flit flit)
{
	if (m_buffer.size() == BUFFER_SIZE)
	{
		log(" Virtual channel: buffer full ");
		return false;
	}
	m_buffer.push_back(flit);
	log(" Virtual channel: flit push back into buffer ");
	return true;
}

Flit VirtualChannel::popfrontFlit()
{
	if (m_buffer.size() == 0)
	{
		log(" Virtual channel: buffer empty ");
	}
	Flit flit{ m_buffer.front() };
	m_buffer.pop_front();
	log(" Virtual channel: flit pop front from buffer ");
	return flit;
}
