#include "NorthPort.h"

void NorthPort::receiveFlit()
{
	if (!m_inFlitBuffer.empty())
	{
		m_virtualChannels.at(m_inFlitBuffer.front().virtualChannel).pushbackFlit(m_inFlitBuffer.front());
		m_inFlitBuffer.pop_front();
	}
}

void NorthPort::computeRoute()
{
	for (auto& virtualChannel : m_virtualChannels)
	{
		if (!virtualChannel.m_buffer.empty())
		{
			if (virtualChannel.m_buffer.front().flitType != FlitType::HeadFlit ||
				virtualChannel.m_buffer.front().flitType != FlitType::HeadTailFlit)
			{
				throw std::runtime_error{ " Nouth port routing error: not head flit " };
			}

			RouterID destination{};
			// lookup mapping table and find the routerID by flit.destination
			for (auto& mappingTableLine : m_mappingTable)
			{
				if (mappingTableLine.NID == virtualChannel.m_buffer.front().destination)
				{
					destination = mappingTableLine.routerID;
					break;
				}
			}

			// XY routing
#if defined(MESH)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_portRouted = PortType::TerminalPort;
					log(" North port: routed to terminal port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::SouthPort;
					log(" North port: routed to south port ");
				}
			}
			else if (destination.x < m_routerID.x)
			{
				virtualChannel.m_portRouted = PortType::WestPort;
				log(" North port: routed to west port ");
			}
			else
			{
				virtualChannel.m_portRouted = PortType::EastPort;
				log(" North port: routed to east port ");
			}
#endif

#if defined(TORUS)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_portRouted = PortType::TerminalPort;
					log(" North port: routed to terminal port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::SouthPort;
					log(" North port: routed to south port ");
				}
			}
			else if (destination.x < m_routerID.x)
			{
				if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
				{
					virtualChannel.m_portRouted = PortType::EastPort; // TORUS
					log(" North port: routed to east port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::WestPort;
					log(" North port: routed to west port ");
				}
			}
			else
			{
				if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
				{
					virtualChannel.m_portRouted = PortType::WestPort; // TORUS
					log(" North port: routed to west port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::EastPort;
					log(" North port: routed to east port ");
				}
			}
#endif
		}
	}
}