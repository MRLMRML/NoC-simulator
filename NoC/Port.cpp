#include "Port.h"

bool Port::isEmpty()
{
	return (m_outFlitBuffer.empty() && m_inFlitBuffer.empty());
}

