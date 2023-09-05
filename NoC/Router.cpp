#include "Router.h"

void Router::runOneStep()
{
	computeRoute();
	allocateVirtualChannel();
	allocateSwitch();
	traverseSwitch();
}

void Router::computeRoute()
{
	routeTerminalPort();	
	routeNorthPort();
	routeSouthPort();
	routeWestPort();
	routeEastPort();
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
				m_terminalPort.m_outputPortRouted = PortType::NorthPort;
				log(" Terminal port: routed to north port ");
			}
			else
			{
				m_terminalPort.m_outputPortRouted = PortType::SouthPort;
				log(" Terminal port: routed to south port ");
			}
		}
		else if (destination.x < m_routerID.x)
		{
			m_terminalPort.m_outputPortRouted = PortType::WestPort;
			log(" Terminal port: routed to west port ");
		}
		else
		{
			m_terminalPort.m_outputPortRouted = PortType::EastPort;
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
					m_terminalPort.m_outputPortRouted = PortType::SouthPort; // TORUS
					log(" Terminal port: routed to south port ");
				}
				else
				{
					m_terminalPort.m_outputPortRouted = PortType::NorthPort;
					log(" Terminal port: routed to north port ");
				}
			}
			else
			{
				if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
				{
					m_terminalPort.m_outputPortRouted = PortType::NorthPort; // TORUS
					log(" Terminal port: routed to north port ");
				}
				else
				{
					m_terminalPort.m_outputPortRouted = PortType::SouthPort;
					log(" Terminal port: routed to south port ");
				}
			}
		}
		else if (destination.x < m_routerID.x)
		{
			if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
			{
				m_terminalPort.m_outputPortRouted = PortType::EastPort; // TORUS
				log(" Terminal port: routed to east port ");
			}
			else
			{
				m_terminalPort.m_outputPortRouted = PortType::WestPort;
				log(" Terminal port: routed to west port ");
			}
		}
		else
		{
			if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
			{
				m_terminalPort.m_outputPortRouted = PortType::WestPort; // TORUS
				log(" Terminal port: routed to west port ");
			}
			else
			{
				m_terminalPort.m_outputPortRouted = PortType::EastPort;
				log(" Terminal port: routed to east port ");
			}
		}
#endif
	}
	m_terminalPort.m_virtualChannelState = VirtualChannelState::V; // R -> V
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
					virtualChannel.m_outputPortRouted = PortType::TerminalPort;
					log(" North port: routed to terminal port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::SouthPort;
					log(" North port: routed to south port ");
				}
			}
			else if (destination.x < m_routerID.x)
			{
				virtualChannel.m_outputPortRouted = PortType::WestPort;
				log(" North port: routed to west port ");
			}
			else
			{
				virtualChannel.m_outputPortRouted = PortType::EastPort;
				log(" North port: routed to east port ");
			}
#endif

#if defined(TORUS)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_outputPortRouted = PortType::TerminalPort;
					log(" North port: routed to terminal port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::SouthPort;
					log(" North port: routed to south port ");
				}
			}
			else if (destination.x < m_routerID.x)
			{
				if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
				{
					virtualChannel.m_outputPortRouted = PortType::EastPort; // TORUS
					log(" North port: routed to east port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::WestPort;
					log(" North port: routed to west port ");
				}
			}
			else
			{
				if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
				{
					virtualChannel.m_outputPortRouted = PortType::WestPort; // TORUS
					log(" North port: routed to west port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::EastPort;
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
					virtualChannel.m_outputPortRouted = PortType::TerminalPort;
					log(" South port: routed to terminal port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::NorthPort;
					log(" South port: routed to north port ");
				}
			}
			else if (destination.x < m_routerID.x)
			{
				virtualChannel.m_outputPortRouted = PortType::WestPort;
				log(" South port: routed to west port ");
			}
			else
			{
				virtualChannel.m_outputPortRouted = PortType::EastPort;
				log(" South port: routed to east port ");
			}
#endif

#if defined(TORUS)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_outputPortRouted = PortType::TerminalPort;
					log(" South port: routed to terminal port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::NorthPort;
					log(" South port: routed to north port ");
				}
			}
			else if (destination.x < m_routerID.x)
			{
				if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
				{
					virtualChannel.m_outputPortRouted = PortType::EastPort; // TORUS
					log(" South port: routed to east port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::WestPort;
					log(" South port: routed to west port ");
				}
			}
			else
			{
				if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
				{
					virtualChannel.m_outputPortRouted = PortType::WestPort; // TORUS
					log(" South port: routed to west port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::EastPort;
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
					virtualChannel.m_outputPortRouted = PortType::TerminalPort;
					log(" West port: routed to terminal port ");
				}
				else if (destination.y < m_routerID.y)
				{
					virtualChannel.m_outputPortRouted = PortType::NorthPort;
					log(" West port: routed to north port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::SouthPort;
					log(" West port: routed to south port ");
				}
			}
			else
			{
				virtualChannel.m_outputPortRouted = PortType::EastPort;
				log(" West port: routed to east port ");
			}
#endif

#if defined(TORUS)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_outputPortRouted = PortType::TerminalPort;
					log(" West port: routed to terminal port ");
				}
				else if (destination.y < m_routerID.y)
				{
					if ((m_routerID.y - destination.y) > (NETWORK_DIMENSION_Y / 2))
					{
						virtualChannel.m_outputPortRouted = PortType::SouthPort; // TORUS
						log(" West port: routed to south port ");
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::NorthPort;
						log(" West port: routed to north port ");
					}
				}
				else
				{
					if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
					{
						virtualChannel.m_outputPortRouted = PortType::NorthPort; //TORUS
						log(" West port: routed to north port ");
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::SouthPort;
						log(" West port: routed to south port ");
					}
				}
			}
			else
			{
				virtualChannel.m_outputPortRouted = PortType::EastPort;
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
					virtualChannel.m_outputPortRouted = PortType::TerminalPort;
					log(" East port: routed to terminal port ");
				}
				else if (destination.y < m_routerID.y)
				{
					virtualChannel.m_outputPortRouted = PortType::NorthPort;
					log(" East port: routed to north port ");
				}
				else
				{
					virtualChannel.m_outputPortRouted = PortType::SouthPort;
					log(" East port: routed to south port ");
				}
			}
			else
			{
				virtualChannel.m_outputPortRouted = PortType::WestPort;
				log(" East port: routed to west port ");
			}
#endif

#if defined(TORUS)
			if (destination.x == m_routerID.x)
			{
				if (destination.y == m_routerID.y)
				{
					virtualChannel.m_outputPortRouted = PortType::TerminalPort;
					log(" East port: routed to terminal port ");
				}
				else if (destination.y < m_routerID.y)
				{
					if ((m_routerID.y - destination.y) > (NETWORK_DIMENSION_Y / 2))
					{
						virtualChannel.m_outputPortRouted = PortType::SouthPort; // TORUS
						log(" East port: routed to south port ");
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::NorthPort;
						log(" East port: routed to north port ");
					}
				}
				else
				{
					if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
					{
						virtualChannel.m_outputPortRouted = PortType::NorthPort; // TORUS
						log(" East port: routed to north port ");
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::SouthPort;
						log(" East port: routed to south port ");
					}
				}
			}
			else
			{
				virtualChannel.m_outputPortRouted = PortType::WestPort;
				log(" East port: routed to west port ");
			}
#endif
		}
		virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
	}
}

void Router::initiateVirtualChannelPriority()
{
	int virtualChannelPriority{};

	m_terminalPort.m_virtualChannelPriority = virtualChannelPriority;

	for (int i{}; i < VC_NUMBER; ++i)
	{
		virtualChannelPriority++;
		m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority = virtualChannelPriority;
	}

	for (int i{}; i < VC_NUMBER; ++i)
	{
		virtualChannelPriority++;
		m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority = virtualChannelPriority;
	}

	for (int i{}; i < VC_NUMBER; ++i)
	{
		virtualChannelPriority++;
		m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority = virtualChannelPriority;
	}

	for (int i{}; i < VC_NUMBER; ++i)
	{
		virtualChannelPriority++;
		m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority = virtualChannelPriority;
	}
}

void Router::allocateVirtualChannel()
{
	// allocate terminal port
	allocateTerminalPortVirtualChannel();

	// allocate north port
	allocateNorthPortVirtualChannel();

	// allocate south port
	allocateSouthPortVirtualChannel();

	// allocate west port
	allocateWestPortVirtualChannel();

	// allocate east port
	allocateEastPortVirtualChannel();

	// activate virtual channels
	activateVirtualChannel();

	// update priority
	updateVirtualChannelPriority();
}

void Router::allocateTerminalPortVirtualChannel()
{
	if (m_terminalPort.m_virtualChannelState == VirtualChannelState::V)
	{
		// if terminal port is routed to north port
		if (m_terminalPort.m_outputPortRouted == PortType::NorthPort)
		{
			for (int i{}; i < VC_NUMBER; ++i)
			{
				if (m_northPort.m_downstreamVirtualChannelStates.at(i)
					== VirtualChannelState::I)
				{
					// change input fields
					m_terminalPort.m_outputVirtualChannelAllocated = i;

					// change output fields
					m_northPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::V;
					m_northPort.m_inputPortRouted.at(i) = PortType::TerminalPort;
					m_northPort.m_inputVirtualChannelAllocated.at(i) = 0; // no virtual channel for ternimal port
					m_northPort.m_inputVirtualChannelPriority.at(i) = m_terminalPort.m_virtualChannelPriority;
					goto end;
				}
			}

			// If there is no I downstream virtual channel states;
			// meaning states are either A or V;
			// do the following round-robin arbitration based on virtual channel priority
			// with DVCS set to V;
			for (int i{}; i < VC_NUMBER; ++i)
			{
				if (m_northPort.m_downstreamVirtualChannelStates.at(i)
					== VirtualChannelState::V)
				{
					// find the item with the lowest virtual channel priority (maximum) 
					// and in V state; record priority and index;
					int priority{};
					int index{};
					for (int j{}; j < VC_NUMBER; ++j)
					{
						if (m_northPort.m_downstreamVirtualChannelStates.at(j) == VirtualChannelState::V
							&& m_northPort.m_inputVirtualChannelPriority.at(j) > priority)
						{
							priority = m_northPort.m_inputVirtualChannelPriority.at(j);
							index = j;
						}
					}

					// compare the lowest priority with the challenger and arbitrate 
					// to the challenger if challenger's priority is higher (smaller)
					if (m_terminalPort.m_virtualChannelPriority < priority)
					{
						// record it in arbitration recorder
						winVirtualChannelArbitration(PortType::TerminalPort, 0);

						// deallocate the loser's vc
						// note that output port cannot be routed to input port with the same type
						// and in this input port case, there is no vc for input terminal port;
						// meaning in other input port cases, there should be four cases  
						if (m_northPort.m_inputPortRouted.at(index) == PortType::SouthPort)
							m_southPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
						if (m_northPort.m_inputPortRouted.at(index) == PortType::WestPort)
							m_westPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
						if (m_northPort.m_inputPortRouted.at(index) == PortType::EastPort)
							m_eastPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

						// allocate vc to the challenger
						// change input fields
						m_terminalPort.m_outputVirtualChannelAllocated = index;

						// change output fields
						m_northPort.m_inputPortRouted.at(index) = PortType::TerminalPort;
						m_northPort.m_inputVirtualChannelAllocated.at(index) = 0; // no virtual channel for ternimal port
						m_northPort.m_inputVirtualChannelPriority.at(index) = m_terminalPort.m_virtualChannelPriority;
						goto end;
					}
				}
			}
		end: // goto statement
		}

		// if terminal port is routed to south port
		if (m_terminalPort.m_outputPortRouted == PortType::SouthPort)
		{
			for (int i{}; i < VC_NUMBER; ++i)
			{
				if (m_southPort.m_downstreamVirtualChannelStates.at(i)
					== VirtualChannelState::I)
				{
					// change input fields
					m_terminalPort.m_outputVirtualChannelAllocated = i;

					// change output fields
					m_southPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::V;
					m_southPort.m_inputPortRouted.at(i) = PortType::TerminalPort;
					m_southPort.m_inputVirtualChannelAllocated.at(i) = 0; // no virtual channel for ternimal port
					m_southPort.m_inputVirtualChannelPriority.at(i) = m_terminalPort.m_virtualChannelPriority;
					goto end;
				}
			}

			// If there is no I downstream virtual channel states;
			// meaning states are either A or V;
			// do the following round-robin arbitration based on virtual channel priority
			// with DVCS set to V;
			for (int i{}; i < VC_NUMBER; ++i)
			{
				if (m_southPort.m_downstreamVirtualChannelStates.at(i)
					== VirtualChannelState::V)
				{
					// find the item with the lowest virtual channel priority (maximum) 
					// and in V state; record priority and index;
					int priority{};
					int index{};
					for (int j{}; j < VC_NUMBER; ++j)
					{
						if (m_southPort.m_downstreamVirtualChannelStates.at(j) == VirtualChannelState::V
							&& m_southPort.m_inputVirtualChannelPriority.at(j) > priority)
						{
							priority = m_southPort.m_inputVirtualChannelPriority.at(j);
							index = j;
						}
					}

					// compare the lowest priority with the challenger and arbitrate 
					// to the challenger if challenger's priority is higher (smaller)
					if (m_terminalPort.m_virtualChannelPriority < priority)
					{
						// record it in arbitration recorder
						winVirtualChannelArbitration(PortType::TerminalPort, 0);

						// deallocate the loser's vc
						// note that output port cannot be routed to input port with the same type
						// and in this input port case, there is no vc for input terminal port;
						// meaning in other input port cases, there should be four cases  
						if (m_southPort.m_inputPortRouted.at(index) == PortType::NorthPort)
							m_northPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
						if (m_southPort.m_inputPortRouted.at(index) == PortType::WestPort)
							m_westPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
						if (m_southPort.m_inputPortRouted.at(index) == PortType::EastPort)
							m_eastPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

						// allocate vc to the challenger
						// change input fields
						m_terminalPort.m_outputVirtualChannelAllocated = index;

						// change output fields
						m_southPort.m_inputPortRouted.at(index) = PortType::TerminalPort;
						m_southPort.m_inputVirtualChannelAllocated.at(index) = 0; // no virtual channel for ternimal port
						m_southPort.m_inputVirtualChannelPriority.at(index) = m_terminalPort.m_virtualChannelPriority;
						goto end;
					}
				}
			}
		end:
		}

		// if terminal port is routed to west port
		if (m_terminalPort.m_outputPortRouted == PortType::WestPort)
		{
			for (int i{}; i < VC_NUMBER; ++i)
			{
				if (m_westPort.m_downstreamVirtualChannelStates.at(i)
					== VirtualChannelState::I)
				{
					// change input fields
					m_terminalPort.m_outputVirtualChannelAllocated = i;

					// change output fields
					m_westPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::V;
					m_westPort.m_inputPortRouted.at(i) = PortType::TerminalPort;
					m_westPort.m_inputVirtualChannelAllocated.at(i) = 0; // no virtual channel for ternimal port
					m_westPort.m_inputVirtualChannelPriority.at(i) = m_terminalPort.m_virtualChannelPriority;
					goto end;
				}
			}

			// If there is no I downstream virtual channel states;
			// meaning states are either A or V;
			// do the following round-robin arbitration based on virtual channel priority
			// with DVCS set to V;
			for (int i{}; i < VC_NUMBER; ++i)
			{
				if (m_westPort.m_downstreamVirtualChannelStates.at(i)
					== VirtualChannelState::V)
				{
					// find the item with the lowest virtual channel priority (maximum) 
					// and in V state; record priority and index;
					int priority{};
					int index{};
					for (int j{}; j < VC_NUMBER; ++j)
					{
						if (m_westPort.m_downstreamVirtualChannelStates.at(j) == VirtualChannelState::V
							&& m_westPort.m_inputVirtualChannelPriority.at(j) > priority)
						{
							priority = m_westPort.m_inputVirtualChannelPriority.at(j);
							index = j;
						}
					}

					// compare the lowest priority with the challenger and arbitrate 
					// to the challenger if challenger's priority is higher (smaller)
					if (m_terminalPort.m_virtualChannelPriority < priority)
					{
						// record it in arbitration recorder
						winVirtualChannelArbitration(PortType::TerminalPort, 0);

						// deallocate the loser's vc
						// note that output port cannot be routed to input port with the same type
						// and in this input port case, there is no vc for input terminal port;
						// meaning in other input port cases, there should be four cases  
						if (m_westPort.m_inputPortRouted.at(index) == PortType::NorthPort)
							m_northPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
						if (m_westPort.m_inputPortRouted.at(index) == PortType::SouthPort)
							m_southPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
						if (m_westPort.m_inputPortRouted.at(index) == PortType::EastPort)
							m_eastPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

						// allocate vc to the challenger
						// change input fields
						m_terminalPort.m_outputVirtualChannelAllocated = index;

						// change output fields
						m_westPort.m_inputPortRouted.at(index) = PortType::TerminalPort;
						m_westPort.m_inputVirtualChannelAllocated.at(index) = 0; // no virtual channel for ternimal port
						m_westPort.m_inputVirtualChannelPriority.at(index) = m_terminalPort.m_virtualChannelPriority;
						goto end;
					}
				}
			}
		end:
		}

		// if terminal port is routed to east port
		if (m_terminalPort.m_outputPortRouted == PortType::EastPort)
		{
			for (int i{}; i < VC_NUMBER; ++i)
			{
				if (m_eastPort.m_downstreamVirtualChannelStates.at(i)
					== VirtualChannelState::I)
				{
					// change input fields
					m_terminalPort.m_outputVirtualChannelAllocated = i;

					// change output fields
					m_eastPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::V;
					m_eastPort.m_inputPortRouted.at(i) = PortType::TerminalPort;
					m_eastPort.m_inputVirtualChannelAllocated.at(i) = 0; // no virtual channel for ternimal port
					m_eastPort.m_inputVirtualChannelPriority.at(i) = m_terminalPort.m_virtualChannelPriority;
					goto end;
				}
			}

			// If there is no I downstream virtual channel states;
			// meaning states are either A or V;
			// do the following round-robin arbitration based on virtual channel priority
			// with DVCS set to V;
			for (int i{}; i < VC_NUMBER; ++i)
			{
				if (m_eastPort.m_downstreamVirtualChannelStates.at(i)
					== VirtualChannelState::V)
				{
					// find the item with the lowest virtual channel priority (maximum) 
					// and in V state; record priority and index;
					int priority{};
					int index{};
					for (int j{}; j < VC_NUMBER; ++j)
					{
						if (m_eastPort.m_downstreamVirtualChannelStates.at(j) == VirtualChannelState::V
							&& m_eastPort.m_inputVirtualChannelPriority.at(j) > priority)
						{
							priority = m_eastPort.m_inputVirtualChannelPriority.at(j);
							index = j;
						}
					}

					// compare the lowest priority with the challenger and arbitrate 
					// to the challenger if challenger's priority is higher (smaller)
					if (m_terminalPort.m_virtualChannelPriority < priority)
					{
						// record it in arbitration recorder
						winVirtualChannelArbitration(PortType::TerminalPort, 0);

						// deallocate the loser's vc
						// note that output port cannot be routed to input port with the same type
						// and in this input port case, there is no vc for input terminal port;
						// meaning in other input port cases, there should be four cases  
						if (m_eastPort.m_inputPortRouted.at(index) == PortType::NorthPort)
							m_northPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
						if (m_eastPort.m_inputPortRouted.at(index) == PortType::SouthPort)
							m_southPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
						if (m_eastPort.m_inputPortRouted.at(index) == PortType::WestPort)
							m_westPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

						// allocate vc to the challenger
						// change input fields
						m_terminalPort.m_outputVirtualChannelAllocated = index;

						// change output fields
						m_eastPort.m_inputPortRouted.at(index) = PortType::TerminalPort;
						m_eastPort.m_inputVirtualChannelAllocated.at(index) = 0; // no virtual channel for ternimal port
						m_eastPort.m_inputVirtualChannelPriority.at(index) = m_terminalPort.m_virtualChannelPriority;
						goto end;
					}
				}
			}
		end:
		}
	}
}

void Router::allocateNorthPortVirtualChannel()
{
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_northPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V)
		{
			// if north port is routed to terminal port
			if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
			{
				if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::I)
				{
					// change input fields
					m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

					// change output fields
					m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::V;
					m_terminalPort.m_inputPortRouted = PortType::NorthPort;
					m_terminalPort.m_inputVirtualChannelAllocated = i;
					m_terminalPort.m_inputVirtualChannelPriority = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::V)
				{
					// compare the priority with the challenger and arbitrate 
					// to the challenger if challenger's priority is higher (smaller)
					int priority{ m_terminalPort.m_inputVirtualChannelPriority };
					if (m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
					{
						// record it in arbitration recorder
						winVirtualChannelArbitration(PortType::NorthPort, i);

						// deallocate the loser's vc
						// note that output port cannot be routed to input port with the same type
						// there should be four cases
						if (m_terminalPort.m_inputPortRouted == PortType::NorthPort)
							m_northPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::SouthPort)
							m_southPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::WestPort)
							m_westPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::EastPort)
							m_eastPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;

						// allocate vc to the challenger
						// change input fields
						m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

						// change output fields
						m_terminalPort.m_inputPortRouted = PortType::NorthPort;
						m_terminalPort.m_inputVirtualChannelAllocated = i;
						m_terminalPort.m_inputVirtualChannelPriority = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
					}
				}
			}

			// if north port is routed to south port
			if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_southPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_southPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_southPort.m_inputPortRouted.at(j) = PortType::NorthPort;
						m_southPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_southPort.m_inputVirtualChannelPriority.at(j) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_southPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_southPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_southPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_southPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::NorthPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_southPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_southPort.m_inputPortRouted.at(index) = PortType::NorthPort;
							m_southPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_southPort.m_inputVirtualChannelPriority.at(index) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}

			// if north port is routed to west port
			if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_westPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_westPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_westPort.m_inputPortRouted.at(j) = PortType::NorthPort;
						m_westPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_westPort.m_inputVirtualChannelPriority.at(j) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_westPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_westPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_westPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_westPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::NorthPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_westPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_westPort.m_inputPortRouted.at(index) = PortType::NorthPort;
							m_westPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_westPort.m_inputVirtualChannelPriority.at(index) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}

			// if north port is routed to east port
			if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_eastPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_eastPort.m_inputPortRouted.at(j) = PortType::NorthPort;
						m_eastPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_eastPort.m_inputVirtualChannelPriority.at(j) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_eastPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_eastPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_eastPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::NorthPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_eastPort.m_inputPortRouted.at(index) = PortType::NorthPort;
							m_eastPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_eastPort.m_inputVirtualChannelPriority.at(index) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}
		}
	}
}

void Router::allocateSouthPortVirtualChannel()
{
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_southPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V)
		{
			// if south port is routed to terminal port
			if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
			{
				if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::I)
				{
					// change input fields
					m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

					// change output fields
					m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::V;
					m_terminalPort.m_inputPortRouted = PortType::SouthPort;
					m_terminalPort.m_inputVirtualChannelAllocated = i;
					m_terminalPort.m_inputVirtualChannelPriority = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::V)
				{
					// compare the priority with the challenger and arbitrate 
					// to the challenger if challenger's priority is higher (smaller)
					int priority{ m_terminalPort.m_inputVirtualChannelPriority };
					if (m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
					{
						// record it in arbitration recorder
						winVirtualChannelArbitration(PortType::SouthPort, i);

						// deallocate the loser's vc
						// note that output port cannot be routed to input port with the same type
						// there should be four cases
						if (m_terminalPort.m_inputPortRouted == PortType::NorthPort)
							m_northPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::SouthPort)
							m_southPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::WestPort)
							m_westPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::EastPort)
							m_eastPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;

						// allocate vc to the challenger
						// change input fields
						m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

						// change output fields
						m_terminalPort.m_inputPortRouted = PortType::SouthPort;
						m_terminalPort.m_inputVirtualChannelAllocated = i;
						m_terminalPort.m_inputVirtualChannelPriority = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
					}
				}
			}

			// if south port is routed to north port
			if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_northPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_northPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_northPort.m_inputPortRouted.at(j) = PortType::SouthPort;
						m_northPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_northPort.m_inputVirtualChannelPriority.at(j) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_northPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_northPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_northPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_northPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::SouthPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_northPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_northPort.m_inputPortRouted.at(index) = PortType::SouthPort;
							m_northPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_northPort.m_inputVirtualChannelPriority.at(index) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}

			// if south port is routed to west port
			if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_westPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_westPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_westPort.m_inputPortRouted.at(j) = PortType::SouthPort;
						m_westPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_westPort.m_inputVirtualChannelPriority.at(j) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_westPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_westPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_westPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_westPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::SouthPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_westPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_westPort.m_inputPortRouted.at(index) = PortType::SouthPort;
							m_westPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_westPort.m_inputVirtualChannelPriority.at(index) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}

			// if south port is routed to east port
			if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_eastPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_eastPort.m_inputPortRouted.at(j) = PortType::SouthPort;
						m_eastPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_eastPort.m_inputVirtualChannelPriority.at(j) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_eastPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_eastPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_eastPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::SouthPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_eastPort.m_inputPortRouted.at(index) = PortType::SouthPort;
							m_eastPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_eastPort.m_inputVirtualChannelPriority.at(index) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}
		}
	}
}

void Router::allocateWestPortVirtualChannel()
{
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_westPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V)
		{
			// if west port is routed to terminal port
			if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
			{
				if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::I)
				{
					// change input fields
					m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

					// change output fields
					m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::V;
					m_terminalPort.m_inputPortRouted = PortType::WestPort;
					m_terminalPort.m_inputVirtualChannelAllocated = i;
					m_terminalPort.m_inputVirtualChannelPriority = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::V)
				{
					// compare the priority with the challenger and arbitrate 
					// to the challenger if challenger's priority is higher (smaller)
					int priority{ m_terminalPort.m_inputVirtualChannelPriority };
					if (m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
					{
						// record it in arbitration recorder
						winVirtualChannelArbitration(PortType::WestPort, i);

						// deallocate the loser's vc
						// note that output port cannot be routed to input port with the same type
						// there should be four cases
						if (m_terminalPort.m_inputPortRouted == PortType::NorthPort)
							m_northPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::SouthPort)
							m_southPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::WestPort)
							m_westPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::EastPort)
							m_eastPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;

						// allocate vc to the challenger
						// change input fields
						m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

						// change output fields
						m_terminalPort.m_inputPortRouted = PortType::WestPort;
						m_terminalPort.m_inputVirtualChannelAllocated = i;
						m_terminalPort.m_inputVirtualChannelPriority = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
					}
				}
			}

			// if west port is routed to north port
			if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_northPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_northPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_northPort.m_inputPortRouted.at(j) = PortType::WestPort;
						m_northPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_northPort.m_inputVirtualChannelPriority.at(j) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_northPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_northPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_northPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_northPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::WestPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_northPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_northPort.m_inputPortRouted.at(index) = PortType::WestPort;
							m_northPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_northPort.m_inputVirtualChannelPriority.at(index) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}

			// if west port is routed to south port
			if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_southPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_southPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_southPort.m_inputPortRouted.at(j) = PortType::WestPort;
						m_southPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_southPort.m_inputVirtualChannelPriority.at(j) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_southPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_southPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_southPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_southPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::WestPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_southPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_southPort.m_inputPortRouted.at(index) = PortType::WestPort;
							m_southPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_southPort.m_inputVirtualChannelPriority.at(index) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}

			// if west port is routed to east port
			if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_eastPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_eastPort.m_inputPortRouted.at(j) = PortType::WestPort;
						m_eastPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_eastPort.m_inputVirtualChannelPriority.at(j) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_eastPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_eastPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_eastPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::WestPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_eastPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_eastPort.m_inputPortRouted.at(index) = PortType::WestPort;
							m_eastPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_eastPort.m_inputVirtualChannelPriority.at(index) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}
		}
	}
}

void Router::allocateEastPortVirtualChannel()
{
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V)
		{
			// if east port is routed to terminal port
			if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
			{
				if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::I)
				{
					// change input fields
					m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

					// change output fields
					m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::V;
					m_terminalPort.m_inputPortRouted = PortType::EastPort;
					m_terminalPort.m_inputVirtualChannelAllocated = i;
					m_terminalPort.m_inputVirtualChannelPriority = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::V)
				{
					// compare the priority with the challenger and arbitrate 
					// to the challenger if challenger's priority is higher (smaller)
					int priority{ m_terminalPort.m_inputVirtualChannelPriority };
					if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
					{
						// record it in arbitration recorder
						winVirtualChannelArbitration(PortType::EastPort, i);

						// deallocate the loser's vc
						// note that output port cannot be routed to input port with the same type
						// there should be four cases
						if (m_terminalPort.m_inputPortRouted == PortType::NorthPort)
							m_northPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::SouthPort)
							m_southPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::WestPort)
							m_westPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;
						if (m_terminalPort.m_inputPortRouted == PortType::EastPort)
							m_eastPort.m_virtualChannels.at(m_terminalPort.m_inputVirtualChannelAllocated).m_outputVirtualChannelAllocated = -1;

						// allocate vc to the challenger
						// change input fields
						m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

						// change output fields
						m_terminalPort.m_inputPortRouted = PortType::EastPort;
						m_terminalPort.m_inputVirtualChannelAllocated = i;
						m_terminalPort.m_inputVirtualChannelPriority = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
					}
				}
			}

			// if east port is routed to north port
			if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_northPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_northPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_northPort.m_inputPortRouted.at(j) = PortType::EastPort;
						m_northPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_northPort.m_inputVirtualChannelPriority.at(j) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_northPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_northPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_northPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_northPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::EastPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_northPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_northPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_northPort.m_inputPortRouted.at(index) = PortType::EastPort;
							m_northPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_northPort.m_inputVirtualChannelPriority.at(index) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}

			// if east port is routed to south port
			if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_southPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_southPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_southPort.m_inputPortRouted.at(j) = PortType::EastPort;
						m_southPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_southPort.m_inputVirtualChannelPriority.at(j) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_southPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_southPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_southPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_southPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::EastPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_southPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::WestPort)
								m_westPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_southPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_southPort.m_inputPortRouted.at(index) = PortType::EastPort;
							m_southPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_southPort.m_inputVirtualChannelPriority.at(index) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}

			// if east port is routed to west port
			if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
			{
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_westPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::I)
					{
						// change input fields
						m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

						// change output fields
						m_westPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
						m_westPort.m_inputPortRouted.at(j) = PortType::EastPort;
						m_westPort.m_inputVirtualChannelAllocated.at(j) = i;
						m_westPort.m_inputVirtualChannelPriority.at(j) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
						goto end;
					}
				}

				// If there is no I downstream virtual channel states;
				// meaning states are either A or V;
				// do the following round-robin arbitration based on virtual channel priority
				// with DVCS set to V;
				for (int j{}; j < VC_NUMBER; ++j)
				{
					if (m_westPort.m_downstreamVirtualChannelStates.at(j)
						== VirtualChannelState::V)
					{
						// find the item with the lowest virtual channel priority (maximum) 
						// and in V state; record priority and index;
						int priority{};
						int index{};
						for (int k{}; k < VC_NUMBER; ++k)
						{
							if (m_westPort.m_downstreamVirtualChannelStates.at(k) == VirtualChannelState::V
								&& m_westPort.m_inputVirtualChannelPriority.at(k) > priority)
							{
								priority = m_westPort.m_inputVirtualChannelPriority.at(k);
								index = k;
							}
						}

						// compare the lowest priority with the challenger and arbitrate 
						// to the challenger if challenger's priority is higher (smaller)
						if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority < priority)
						{
							// record it in arbitration recorder
							winVirtualChannelArbitration(PortType::EastPort, i);

							// deallocate the loser's vc
							// note that output port cannot be routed to input port with the same type
							// there should be four cases
							if (m_westPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
								m_terminalPort.m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::NorthPort)
								m_northPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::SouthPort)
								m_southPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
							if (m_westPort.m_inputPortRouted.at(index) == PortType::EastPort)
								m_eastPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

							// allocate vc to the challenger
							// change input fields
							m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

							// change output fields
							m_westPort.m_inputPortRouted.at(index) = PortType::EastPort;
							m_westPort.m_inputVirtualChannelAllocated.at(index) = i;
							m_westPort.m_inputVirtualChannelPriority.at(index) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto end;
						}
					}
				}
			end:
			}
		}
	}
}

void Router::winVirtualChannelArbitration(const PortType port, const int virtualChannel)
{
	m_virtualChannelArbitrationRecorder.push_back({ port, virtualChannel });
}

void Router::activateVirtualChannel()
{
	// activate terminal port
	if (m_terminalPort.m_virtualChannelState == VirtualChannelState::V
		&& m_terminalPort.m_outputVirtualChannelAllocated != -1)
		m_terminalPort.m_virtualChannelState = VirtualChannelState::A;
	if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::V)
		m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::A;

	// activate north port
	for (int i{}; i < m_northPort.m_virtualChannels.size(); ++i)
	{
		if (m_northPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V
			&& m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated != -1)
			m_northPort.m_virtualChannels.at(i).m_virtualChannelState = VirtualChannelState::A;
		if (m_northPort.m_downstreamVirtualChannelStates.at(i) == VirtualChannelState::V)
			m_northPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::A;
	}

	// activate south port
	for (int i{}; i < m_southPort.m_virtualChannels.size(); ++i)
	{
		if (m_southPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V
			&& m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated != -1)
			m_southPort.m_virtualChannels.at(i).m_virtualChannelState = VirtualChannelState::A;
		if (m_southPort.m_downstreamVirtualChannelStates.at(i) == VirtualChannelState::V)
			m_southPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::A;
	}

	// activate west port
	for (int i{}; i < m_westPort.m_virtualChannels.size(); ++i)
	{
		if (m_westPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V
			&& m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated != -1)
			m_westPort.m_virtualChannels.at(i).m_virtualChannelState = VirtualChannelState::A;
		if (m_westPort.m_downstreamVirtualChannelStates.at(i) == VirtualChannelState::V)
			m_westPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::A;
	}

	// activate east port
	for (int i{}; i < m_eastPort.m_virtualChannels.size(); ++i)
	{
		if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V
			&& m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated != -1)
			m_eastPort.m_virtualChannels.at(i).m_virtualChannelState = VirtualChannelState::A;
		if (m_eastPort.m_downstreamVirtualChannelStates.at(i) == VirtualChannelState::V)
			m_eastPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::A;
	}
}

void Router::updateVirtualChannelPriority()
{
	int counter{};

	// update terminal port
	m_terminalPort.m_virtualChannelPriority -= counter;
	for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
	{
		if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::NorthPort
			&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == 0)
		{
			m_terminalPort.m_virtualChannelPriority = (1 + VC_NUMBER * BUFFER_SIZE) - m_virtualChannelArbitrationRecorder.size() + j;
			counter++;
			break;
		}
	}

	// update north port
	for (int i{}; i < m_northPort.m_virtualChannels.size(); ++i)
	{
		m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority -= counter;
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::NorthPort 
				&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == i)
			{
				m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority = (1 + VC_NUMBER * BUFFER_SIZE) - m_virtualChannelArbitrationRecorder.size() + j;
				counter++;
				break;
			}
		}
	}

	// update south port
	for (int i{}; i < m_southPort.m_virtualChannels.size(); ++i)
	{
		m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority -= counter;
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::NorthPort
				&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == i)
			{
				m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority = (1 + VC_NUMBER * BUFFER_SIZE) - m_virtualChannelArbitrationRecorder.size() + j;
				counter++;
				break;
			}
		}
	}

	// update west port
	for (int i{}; i < m_westPort.m_virtualChannels.size(); ++i)
	{
		m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority -= counter;
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::NorthPort
				&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == i)
			{
				m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority = (1 + VC_NUMBER * BUFFER_SIZE) - m_virtualChannelArbitrationRecorder.size() + j;
				counter++;
				break;
			}
		}
	}

	// update east port
	for (int i{}; i < m_eastPort.m_virtualChannels.size(); ++i)
	{
		m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority -= counter;
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::NorthPort
				&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == i)
			{
				m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority = (1 + VC_NUMBER * BUFFER_SIZE) - m_virtualChannelArbitrationRecorder.size() + j;
				counter++;
				break;
			}
		}
	}

	// clear the arbitration recorder
	m_virtualChannelArbitrationRecorder.clear(); 
}

void Router::initiateSwitchPriority()
{
	// terminal port
	m_terminalPort.m_switchPriorityGlobal = 0;

	// north port
	for (int i{}; i < VC_NUMBER; ++i)
		m_northPort.m_virtualChannels.at(i).m_switchPriorityLocal = i;
	m_northPort.m_switchPriorityGlobal = 1;

	// south port
	for (int i{}; i < VC_NUMBER; ++i)
		m_southPort.m_virtualChannels.at(i).m_switchPriorityLocal = i;
	m_southPort.m_switchPriorityGlobal = 2;

	// west port
	for (int i{}; i < VC_NUMBER; ++i)
		m_westPort.m_virtualChannels.at(i).m_switchPriorityLocal = i;
	m_westPort.m_switchPriorityGlobal = 3;

	// east port
	for (int i{}; i < VC_NUMBER; ++i)
		m_eastPort.m_virtualChannels.at(i).m_switchPriorityLocal = i;
	m_eastPort.m_switchPriorityGlobal = 4;
}

void Router::allocateSwitch()
{
	// allocate terminal port
	if (m_terminalPort.m_virtualChannelState == VirtualChannelState::A
		&& )

	// allocate north port
	

	// allocate south port
	

	// allocate west port
	

	// allocate east port


}

void Router::winSwitchArbitration(const PortType port)
{
	m_switchArbitrationRecorder.push_back(port);
}