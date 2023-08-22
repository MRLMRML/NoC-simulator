#include "Router.h"

void Router::runOneStep()
{
	computeRoute();
	allocateVirtualChannel();
	allocateSwitchAndTraverseSwitch();
}


void Router::computeRoute()
{
	routeNorthPort();
	routeSouthPort();
	routeWestPort();
	routeEastPort();
	routeTerminalPort();
}

void Router::initiateVCPT()
{
	
}


void Router::allocateVirtualChannel()
{
	for 
}

void Router::routeNorthPort()
{
	m_northPort.receiveFlit();

	for (auto& virtualChannel : m_northPort.m_virtualChannels)
	{
		if (virtualChannel.m_virtualChannelState == VirtualChannelState::R)
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
		virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
	}
}

void Router::routeSouthPort()
{
	m_southPort.receiveFlit();

	for (auto& virtualChannel : m_southPort.m_virtualChannels)
	{
		if (virtualChannel.m_virtualChannelState == VirtualChannelState::R)
		{
			if (virtualChannel.m_buffer.front().flitType != FlitType::HeadFlit ||
				virtualChannel.m_buffer.front().flitType != FlitType::HeadTailFlit)
			{
				throw std::runtime_error{ " South port routing error: not head flit " };
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
					log(" South port: routed to terminal port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::NorthPort;
					log(" South port: routed to north port ");
				}
			}
			else if (destination.x < m_routerID.x)
			{
				virtualChannel.m_portRouted = PortType::WestPort;
				log(" South port: routed to west port ");
			}
			else
			{
				virtualChannel.m_portRouted = PortType::EastPort;
				log(" South port: routed to east port ");
			}
#endif

#if defined(TORUS)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_portRouted = PortType::TerminalPort;
					log(" South port: routed to terminal port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::NorthPort;
					log(" South port: routed to north port ");
				}
			}
			else if (destination.x < m_routerID.x)
			{
				if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
				{
					virtualChannel.m_portRouted = PortType::EastPort; // TORUS
					log(" South port: routed to east port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::WestPort;
					log(" South port: routed to west port ");
				}
			}
			else
			{
				if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
				{
					virtualChannel.m_portRouted = PortType::WestPort; // TORUS
					log(" South port: routed to west port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::EastPort;
					log(" South port: routed to east port ");
				}
			}
#endif
		}
		virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
	}
}

void Router::routeWestPort()
{
	m_westPort.receiveFlit();

	for (auto& virtualChannel : m_westPort.m_virtualChannels)
	{
		if (virtualChannel.m_virtualChannelState == VirtualChannelState::R)
		{
			if (virtualChannel.m_buffer.front().flitType != FlitType::HeadFlit ||
				virtualChannel.m_buffer.front().flitType != FlitType::HeadTailFlit)
			{
				throw std::runtime_error{ " West port routing error: not head flit " };
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
					log(" West port: routed to terminal port ");
				}
				else if (destination.y < m_routerID.y)
				{
					virtualChannel.m_portRouted = PortType::NorthPort;
					log(" West port: routed to north port ");
				}
				else
				{
					virtualChannel.m_portRouted = PortType::SouthPort;
					log(" West port: routed to south port ");
				}
			}
			else
			{
				virtualChannel.m_portRouted = PortType::EastPort;
				log(" West port: routed to east port ");
			}
#endif

#if defined(TORUS)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_portRouted = PortType::TerminalPort;
					log(" West port: routed to terminal port ");
				}
				else if (destination.y < m_routerID.y)
				{
					if ((m_routerID.y - destination.y) > (NETWORK_DIMENSION_Y / 2))
					{
						virtualChannel.m_portRouted = PortType::SouthPort; // TORUS
						log(" West port: routed to south port ");
					}
					else
					{
						virtualChannel.m_portRouted = PortType::NorthPort;
						log(" West port: routed to north port ");
					}
				}
				else
				{
					if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
					{
						virtualChannel.m_portRouted = PortType::NorthPort; //TORUS
						log(" West port: routed to north port ");
					}
					else
					{
						virtualChannel.m_portRouted = PortType::SouthPort;
						log(" West port: routed to south port ");
					}
				}
			}
			else
			{
				virtualChannel.m_portRouted = PortType::EastPort;
				log(" West port: routed to east port ");
			}
#endif
		}
		virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
	}
}

void Router::routeEastPort()
{
	m_eastPort.receiveFlit();

	for (auto& virtualChannel : m_eastPort.m_virtualChannels)
	{
		if (virtualChannel.m_virtualChannelState == VirtualChannelState::R)
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
		virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
	}
}

void Router::routeTerminalPort()
{
	if (m_terminalPort.m_virtualChannelState == VirtualChannelState::R)
	{
		if (m_terminalPort.m_inFlitBuffer.front().flitType != FlitType::HeadFlit ||
			m_terminalPort.m_inFlitBuffer.front().flitType != FlitType::HeadTailFlit)
		{
			throw std::runtime_error{ " Terminal port routing error: not head flit " };
		}

		RouterID destination{};
		// lookup mapping table and find the routerID by flit.destination
		for (auto& mappingTableLine : m_mappingTable)
		{
			if (mappingTableLine.NID == m_terminalPort.m_inFlitBuffer.front().destination)
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
				m_terminalPort.m_portRouted = PortType::NorthPort;
				log(" Terminal port: routed to north port ");
			}
			else
			{
				m_terminalPort.m_portRouted = PortType::SouthPort;
				log(" Terminal port: routed to south port ");
			}
		}
		else if (destination.x < m_routerID.x)
		{
			m_terminalPort.m_portRouted = PortType::WestPort;
			log(" Terminal port: routed to west port ");
		}
		else
		{
			m_terminalPort.m_portRouted = PortType::EastPort;
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
					m_terminalPort.m_portRouted = PortType::SouthPort; // TORUS
					log(" Terminal port: routed to south port ");
				}
				else
				{
					m_terminalPort.m_portRouted = PortType::NorthPort;
					log(" Terminal port: routed to north port ");
				}
			}
			else
			{
				if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
				{
					m_terminalPort.m_portRouted = PortType::NorthPort; // TORUS
					log(" Terminal port: routed to north port ");
				}
				else
				{
					m_terminalPort.m_portRouted = PortType::SouthPort;
					log(" Terminal port: routed to south port ");
				}
			}
		}
		else if (destination.x < m_routerID.x)
		{
			if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
			{
				m_terminalPort.m_portRouted = PortType::EastPort; // TORUS
				log(" Terminal port: routed to east port ");
			}
			else
			{
				m_terminalPort.m_portRouted = PortType::WestPort;
				log(" Terminal port: routed to west port ");
			}
		}
		else
		{
			if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
			{
				m_terminalPort.m_portRouted = PortType::WestPort; // TORUS
				log(" Terminal port: routed to west port ");
			}
			else
			{
				m_terminalPort.m_portRouted = PortType::EastPort;
				log(" Terminal port: routed to east port ");
			}
		}
#endif
	}
	m_terminalPort.m_virtualChannelState = VirtualChannelState::V; // R -> V
}
