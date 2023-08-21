#include "EastPort.h"

void EastPort::computeRoute()
{
	for (auto& virtualChannel : m_virtualChannels)
	{
		if (!virtualChannel.m_buffer.empty())
		{
			if (virtualChannel.m_buffer.front().flitType != FlitType::HeadFlit ||
				virtualChannel.m_buffer.front().flitType != FlitType::HeadTailFlit)
			{
				throw std::runtime_error{ " East port routing error: not head flit " };
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
					log(" East port: routed to terminal port ");
				}
				else if (destination.y < m_routerID.y)
				{
					virtualChannel.m_portRouted = PortType::NorthPort;
					log(" East port: routed to north port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::SouthPort;
					log(" East port: routed to south port ");
				}
			}
			else
			{
				virtualChannel.m_portRouted = PortType::WestPort;
				log(" East port: routed to west port ");
			}
#endif

#if defined(TORUS)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_portRouted = PortType::TerminalPort;
					log(" East port: routed to terminal port ");
				}
				else if (destination.y < m_routerID.y)
				{
					if ((m_routerID.y - destination.y) > (NETWORK_DIMENSION_Y / 2))
					{
						virtualChannel.m_portRouted = PortType::SouthPort; // TORUS
						log(" East port: routed to south port ");
					}
					else
					{
						virtualChannel.m_portRouted = PortType::NorthPort;
						log(" East port: routed to north port ");
					}
				}
				else
				{
					if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
					{
						virtualChannel.m_portRouted = PortType::NorthPort; // TORUS
						log(" East port: routed to north port ");
					}
					else
					{
						virtualChannel.m_portRouted = PortType::SouthPort;
						log(" East port: routed to south port ");
					}
				}
			}
			else
			{
				virtualChannel.m_portRouted = PortType::WestPort;
				log(" East port: routed to west port ");
			}
#endif
		}
	}
}