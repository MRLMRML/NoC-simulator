#include "VirtualChannel.h"

void VirtualChannel::pushbackFlit(const Flit flit)
{
	if (m_virtualChannel.size() == BUFFER_SIZE)
		throw std::runtime_error{ " Virtual channel error: buffer full " };

	m_virtualChannel.push_back(flit);
}

void VirtualChannel::popfrontFlit()
{
	m_virtualChannel.pop_front();
}
