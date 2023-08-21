#include "TerminalPort.h"

void TerminalPort::computeRoute()
{
	if (!inFlitBuffer.empty())
	{
		if (inFlitBuffer.front().flitType != FlitType::HeadFlit ||
			inFlitBuffer.front().flitType != FlitType::HeadTailFlit)
		{
			throw std::runtime_error{ " Terminal port routing error: not head flit " };
		}

		RouterID destination{};
		// lookup mapping table and find the routerID by flit.destination
		for (auto& mappingTableLine : m_mappingTable)
		{
			if (mappingTableLine.NID == inFlitBuffer.front().destination)
			{
				destination = mappingTableLine.routerID;
				break;
			}
		}

		// XY routing
#if defined(MESH)
		if (destination.x == m_routerID.x)
		{
			if (destination.y < m_routerID.y)
			{
				m_portRouted = PortType::NorthPort;
				log(" Terminal port: routed to north port ");
			}
			else
			{
				m_portRouted = PortType::SouthPort;
				log(" Terminal port: routed to south port ");
			}
		}
		else if (destination.x < m_routerID.x)
		{
			m_portRouted = PortType::WestPort;
			log(" Terminal port: routed to west port ");
		}
		else
		{
			m_portRouted = PortType::EastPort;
			log(" Terminal port: routed to east port ");
		}
#endif

#if defined(TORUS)
		if (destination.x == m_routerID.x)
		{
			if (destination.y < m_routerID.y)
			{
				if ((m_routerID.y - destination.y) > (NETWORK_DIMENSION_Y / 2))
				{
					m_portRouted = PortType::SouthPort; // TORUS
					log(" Terminal port: routed to south port ");
				}
				else
				{
					m_portRouted = PortType::NorthPort;
					log(" Terminal port: routed to north port ");
				}
			}
			else
			{
				if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
				{
					m_portRouted = PortType::NorthPort; // TORUS
					log(" Terminal port: routed to north port ");
				}
				else
				{
					m_portRouted = PortType::SouthPort;
					log(" Terminal port: routed to south port ");
				}
			}
		}
		else if (destination.x < m_routerID.x)
		{
			if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
			{
				m_portRouted = PortType::EastPort; // TORUS
				log(" Terminal port: routed to east port ");
			}
			else
			{
				m_portRouted = PortType::WestPort;
				log(" Terminal port: routed to west port ");
			}
		}
		else
		{
			if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
			{
				m_portRouted = PortType::WestPort; // TORUS
				log(" Terminal port: routed to west port ");
			}
			else
			{
				m_portRouted = PortType::EastPort;
				log(" Terminal port: routed to east port ");
			}
		}
#endif
	}
}