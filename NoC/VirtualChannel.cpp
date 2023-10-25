#include "VirtualChannel.h"

bool VirtualChannel::pushbackFlit(Flit flit)
{
	if (m_buffer.size() == BUFFER_SIZE)
	{
		logDebug(" Virtual channel: buffer full ");
		return false;
	}
	m_buffer.push_back(flit);
	logDebug(" Virtual channel: flit push back into buffer ");
	return true;
}

Flit VirtualChannel::popfrontFlit()
{
	Flit flit{ m_buffer.front() };
	m_buffer.pop_front();
	logDebug(" Virtual channel: flit pop front from buffer ");
	return flit;
}

bool VirtualChannel::isEmpty()
{
	return m_buffer.empty();
}

