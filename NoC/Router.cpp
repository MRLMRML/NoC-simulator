#include "Router.h"

void Router::runOneStep()
{
	viewData(); // debug
	computeRoute();
	allocateVirtualChannel();
	traverseThisSwitchThenAllocateNextSwitch();
	synchronizeTriggerClocks();
	synchronizeExecutionClocks();
	viewData(); // debug
}

void Router::receiveFlitAndCredit()
{
	m_terminalPort.receiveFlit();

	m_northPort.receiveFlit();
	if (m_northPort.receiveCredit())
		m_northPort.m_downstreamVirtualChannelStates.at(m_northPort.m_inCreditRegister.front().virtualChannel) = VirtualChannelState::I;
	if (!m_northPort.m_inCreditRegister.empty())
		m_northPort.m_inCreditRegister.pop_front();

	m_southPort.receiveFlit();
	if (m_southPort.receiveCredit())
		m_southPort.m_downstreamVirtualChannelStates.at(m_southPort.m_inCreditRegister.front().virtualChannel) = VirtualChannelState::I;
	if (!m_southPort.m_inCreditRegister.empty())
		m_southPort.m_inCreditRegister.pop_front();

	m_westPort.receiveFlit();
	if (m_westPort.receiveCredit())
		m_westPort.m_downstreamVirtualChannelStates.at(m_westPort.m_inCreditRegister.front().virtualChannel) = VirtualChannelState::I;
	if (!m_westPort.m_inCreditRegister.empty())
		m_westPort.m_inCreditRegister.pop_front();

	m_eastPort.receiveFlit();
	if (m_eastPort.receiveCredit())
		m_eastPort.m_downstreamVirtualChannelStates.at(m_eastPort.m_inCreditRegister.front().virtualChannel) = VirtualChannelState::I;
	if (!m_eastPort.m_inCreditRegister.empty())
		m_eastPort.m_inCreditRegister.pop_front();
}

void Router::computeRoute()
{
	receiveFlitAndCredit();

	routeTerminalPort();
	routeNorthPort();
	routeSouthPort();
	routeWestPort();
	routeEastPort();
}

void Router::routeTerminalPort()
{
	if (m_terminalPort.m_localClock.triggerLocalEvent())
	{
		if (m_terminalPort.m_virtualChannel.m_virtualChannelState == VirtualChannelState::R)
		{
			if (!m_terminalPort.m_localClock.isWaitingForExecution())
			{
				m_terminalPort.m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_RC - 1);
				m_terminalPort.m_localClock.toggleWaitingForExecution();
			}

			if (m_terminalPort.m_localClock.executeLocalEvent())
			{
				RouterID destination{};
				// lookup mapping table and find the routerID by flit.destination
				for (auto& mappingTableLine : m_mappingTable)
				{
					if (mappingTableLine.NID == m_terminalPort.m_virtualChannel.m_buffer.front().destination)
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
						m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::NorthPort;
						logDebug(" Terminal port: routed to north port ");
					}
					else
					{
						m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::SouthPort;
						logDebug(" Terminal port: routed to south port ");
					}
				}
				else if (destination.x < m_routerID.x)
				{
					m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::WestPort;
					logDebug(" Terminal port: routed to west port ");
				}
				else
				{
					m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::EastPort;
					logDebug(" Terminal port: routed to east port ");
				}
#endif

#if defined(TORUS)
				if (destination.x == m_routerID.x)
				{
					if (destination.y < m_routerID.y)
					{
						if ((m_routerID.y - destination.y) > (NETWORK_DIMENSION_Y / 2))
						{
							m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::SouthPort; // TORUS
							logDebug(" Terminal port: routed to south port ");
						}
						else
						{
							m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::NorthPort;
							logDebug(" Terminal port: routed to north port ");
						}
					}
					else
					{
						if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
						{
							m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::NorthPort; // TORUS
							logDebug(" Terminal port: routed to north port ");
						}
						else
						{
							m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::SouthPort;
							logDebug(" Terminal port: routed to south port ");
						}
					}
				}
				else if (destination.x < m_routerID.x)
				{
					if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
					{
						m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::EastPort; // TORUS
						logDebug(" Terminal port: routed to east port ");
					}
					else
					{
						m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::WestPort;
						logDebug(" Terminal port: routed to west port ");
					}
				}
				else
				{
					if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
					{
						m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::WestPort; // TORUS
						logDebug(" Terminal port: routed to west port ");
					}
					else
					{
						m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::EastPort;
						logDebug(" Terminal port: routed to east port ");
					}
				}
#endif
				m_terminalPort.m_virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
				m_terminalPort.m_localClock.tickTriggerClock(PERIOD_ROUTER_RC - EXECUTION_TIME_ROUTER_RC + 1);
				m_terminalPort.m_localClock.toggleWaitingForExecution();
			}
		}
	}
}

void Router::routeNorthPort()
{
	for (auto& virtualChannel : m_northPort.m_virtualChannels)
	{
		if (virtualChannel.m_localClock.triggerLocalEvent())
		{
			if (virtualChannel.m_virtualChannelState == VirtualChannelState::R)
			{
				if (!virtualChannel.m_localClock.isWaitingForExecution())
				{
					virtualChannel.m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_RC - 1);
					virtualChannel.m_localClock.toggleWaitingForExecution();
				}

				if (virtualChannel.m_localClock.executeLocalEvent())
				{
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
							logDebug(" North port: routed to terminal port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::SouthPort;
							logDebug(" North port: routed to south port ");
						}
					}
					else if (destination.x < m_routerID.x)
					{
						virtualChannel.m_outputPortRouted = PortType::WestPort;
						logDebug(" North port: routed to west port ");
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::EastPort;
						logDebug(" North port: routed to east port ");
					}
		#endif

		#if defined(TORUS)
					if (destination.x == m_routerID.x)
					{
						if (destination.y == m_routerID.y)
						{
							virtualChannel.m_outputPortRouted = PortType::TerminalPort;
							logDebug(" North port: routed to terminal port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::SouthPort;
							logDebug(" North port: routed to south port ");
						}
					}
					else if (destination.x < m_routerID.x)
					{
						if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
						{
							virtualChannel.m_outputPortRouted = PortType::EastPort; // TORUS
							logDebug(" North port: routed to east port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::WestPort;
							logDebug(" North port: routed to west port ");
						}
					}
					else
					{
						if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
						{
							virtualChannel.m_outputPortRouted = PortType::WestPort; // TORUS
							logDebug(" North port: routed to west port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::EastPort;
							logDebug(" North port: routed to east port ");
						}
					}
		#endif
					virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
					virtualChannel.m_localClock.tickTriggerClock(PERIOD_ROUTER_RC - EXECUTION_TIME_ROUTER_RC + 1);
					virtualChannel.m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::routeSouthPort()
{
	for (auto& virtualChannel : m_southPort.m_virtualChannels)
	{
		if (virtualChannel.m_localClock.triggerLocalEvent())
		{
			if (virtualChannel.m_virtualChannelState == VirtualChannelState::R)
			{
				if (!virtualChannel.m_localClock.isWaitingForExecution())
				{
					virtualChannel.m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_RC - 1);
					virtualChannel.m_localClock.toggleWaitingForExecution();
				}

				if (virtualChannel.m_localClock.executeLocalEvent())
				{
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
							logDebug(" South port: routed to terminal port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::NorthPort;
							logDebug(" South port: routed to north port ");
						}
					}
					else if (destination.x < m_routerID.x)
					{
						virtualChannel.m_outputPortRouted = PortType::WestPort;
						logDebug(" South port: routed to west port ");
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::EastPort;
						logDebug(" South port: routed to east port ");
					}
		#endif

		#if defined(TORUS)
					if (destination.x == m_routerID.x)
					{
						if (destination.y == m_routerID.y)
						{
							virtualChannel.m_outputPortRouted = PortType::TerminalPort;
							logDebug(" South port: routed to terminal port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::NorthPort;
							logDebug(" South port: routed to north port ");
						}
					}
					else if (destination.x < m_routerID.x)
					{
						if ((m_routerID.x - destination.x) > (NETWORK_DIMENSION_X / 2))
						{
							virtualChannel.m_outputPortRouted = PortType::EastPort; // TORUS
							logDebug(" South port: routed to east port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::WestPort;
							logDebug(" South port: routed to west port ");
						}
					}
					else
					{
						if ((destination.x - m_routerID.x) > (NETWORK_DIMENSION_X / 2))
						{
							virtualChannel.m_outputPortRouted = PortType::WestPort; // TORUS
							logDebug(" South port: routed to west port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::EastPort;
							logDebug(" South port: routed to east port ");
						}
					}
		#endif
					virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
					virtualChannel.m_localClock.tickTriggerClock(PERIOD_ROUTER_RC - EXECUTION_TIME_ROUTER_RC + 1);
					virtualChannel.m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::routeWestPort()
{
	for (auto& virtualChannel : m_westPort.m_virtualChannels)
	{
		if (virtualChannel.m_localClock.triggerLocalEvent())
		{
			if (virtualChannel.m_virtualChannelState == VirtualChannelState::R)
			{
				if (!virtualChannel.m_localClock.isWaitingForExecution())
				{
					virtualChannel.m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_RC - 1);
					virtualChannel.m_localClock.toggleWaitingForExecution();
				}

				if (virtualChannel.m_localClock.executeLocalEvent())
				{
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
							logDebug(" West port: routed to terminal port ");
						}
						else if (destination.y < m_routerID.y)
						{
							virtualChannel.m_outputPortRouted = PortType::NorthPort;
							logDebug(" West port: routed to north port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::SouthPort;
							logDebug(" West port: routed to south port ");
						}
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::EastPort;
						logDebug(" West port: routed to east port ");
					}
		#endif

		#if defined(TORUS)
					if (destination.x == m_routerID.x)
					{
						if (destination.y == m_routerID.y)
						{
							virtualChannel.m_outputPortRouted = PortType::TerminalPort;
							logDebug(" West port: routed to terminal port ");
						}
						else if (destination.y < m_routerID.y)
						{
							if ((m_routerID.y - destination.y) > (NETWORK_DIMENSION_Y / 2))
							{
								virtualChannel.m_outputPortRouted = PortType::SouthPort; // TORUS
								logDebug(" West port: routed to south port ");
							}
							else
							{
								virtualChannel.m_outputPortRouted = PortType::NorthPort;
								logDebug(" West port: routed to north port ");
							}
						}
						else
						{
							if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
							{
								virtualChannel.m_outputPortRouted = PortType::NorthPort; //TORUS
								logDebug(" West port: routed to north port ");
							}
							else
							{
								virtualChannel.m_outputPortRouted = PortType::SouthPort;
								logDebug(" West port: routed to south port ");
							}
						}
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::EastPort;
						logDebug(" West port: routed to east port ");
					}
		#endif
					virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
					virtualChannel.m_localClock.tickTriggerClock(PERIOD_ROUTER_RC - EXECUTION_TIME_ROUTER_RC + 1);
					virtualChannel.m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::routeEastPort()
{
	for (auto& virtualChannel : m_eastPort.m_virtualChannels)
	{
		if (virtualChannel.m_localClock.triggerLocalEvent())
		{
			if (virtualChannel.m_virtualChannelState == VirtualChannelState::R)
			{
				if (!virtualChannel.m_localClock.isWaitingForExecution())
				{
					virtualChannel.m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_RC - 1);
					virtualChannel.m_localClock.toggleWaitingForExecution();
				}

				if (virtualChannel.m_localClock.executeLocalEvent())
				{
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
							logDebug(" East port: routed to terminal port ");
						}
						else if (destination.y < m_routerID.y)
						{
							virtualChannel.m_outputPortRouted = PortType::NorthPort;
							logDebug(" East port: routed to north port ");
						}
						else
						{
							virtualChannel.m_outputPortRouted = PortType::SouthPort;
							logDebug(" East port: routed to south port ");
						}
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::WestPort;
						logDebug(" East port: routed to west port ");
					}
		#endif

		#if defined(TORUS)
					if (destination.x == m_routerID.x)
					{
						if (destination.y == m_routerID.y)
						{
							virtualChannel.m_outputPortRouted = PortType::TerminalPort;
							logDebug(" East port: routed to terminal port ");
						}
						else if (destination.y < m_routerID.y)
						{
							if ((m_routerID.y - destination.y) > (NETWORK_DIMENSION_Y / 2))
							{
								virtualChannel.m_outputPortRouted = PortType::SouthPort; // TORUS
								logDebug(" East port: routed to south port ");
							}
							else
							{
								virtualChannel.m_outputPortRouted = PortType::NorthPort;
								logDebug(" East port: routed to north port ");
							}
						}
						else
						{
							if ((destination.y - m_routerID.y) > (NETWORK_DIMENSION_Y / 2))
							{
								virtualChannel.m_outputPortRouted = PortType::NorthPort; // TORUS
								logDebug(" East port: routed to north port ");
							}
							else
							{
								virtualChannel.m_outputPortRouted = PortType::SouthPort;
								logDebug(" East port: routed to south port ");
							}
						}
					}
					else
					{
						virtualChannel.m_outputPortRouted = PortType::WestPort;
						logDebug(" East port: routed to west port ");
					}
		#endif
					virtualChannel.m_virtualChannelState = VirtualChannelState::V; // R -> V
					virtualChannel.m_localClock.tickTriggerClock(PERIOD_ROUTER_RC - EXECUTION_TIME_ROUTER_RC + 1);
					virtualChannel.m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::initiateVirtualChannelPriority()
{
	int virtualChannelPriority{};

	m_terminalPort.m_virtualChannel.m_virtualChannelPriority = virtualChannelPriority;

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
	receiveFlitAndCredit();

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
	if (m_terminalPort.m_localClock.triggerLocalEvent())
	{
		if (m_terminalPort.m_virtualChannel.m_virtualChannelState == VirtualChannelState::V)
		{
			if (!m_terminalPort.m_localClock.isWaitingForExecution())
			{
				m_terminalPort.m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_VA - 1);
				m_terminalPort.m_localClock.toggleWaitingForExecution();
			}

			if (m_terminalPort.m_localClock.executeLocalEvent())
			{
				// if terminal port is routed to north port
				if (m_terminalPort.m_virtualChannel.m_outputPortRouted == PortType::NorthPort)
				{
					for (int i{}; i < VC_NUMBER; ++i)
					{
						if (m_northPort.m_downstreamVirtualChannelStates.at(i)
							== VirtualChannelState::I)
						{
							// insert an entry in m_virtualChannelArbitrationRecorder
							winVirtualChannelArbitration(PortType::TerminalPort, 0, m_terminalPort.m_virtualChannel.m_virtualChannelPriority);

							// change input fields
							m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = i;

							// change output fields
							m_northPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::V;
							m_northPort.m_inputPortRouted.at(i) = PortType::TerminalPort;
							m_northPort.m_inputVirtualChannelAllocated.at(i) = 0; // no virtual channel for ternimal port
							m_northPort.m_inputVirtualChannelPriority.at(i) = m_terminalPort.m_virtualChannel.m_virtualChannelPriority;
							goto allocateTerminalToNorthDone;
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
							if (m_terminalPort.m_virtualChannel.m_virtualChannelPriority < priority)
							{
								// find loser's record and delete it in m_virtualChannelArbitrationRecorder
								loseVirtualChannelArbitration(m_northPort.m_inputPortRouted.at(index), m_northPort.m_inputVirtualChannelAllocated.at(index), m_northPort.m_inputVirtualChannelPriority.at(index));
						
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

								// record winner in arbitration recorder
								winVirtualChannelArbitration(PortType::TerminalPort, 0, m_terminalPort.m_virtualChannel.m_virtualChannelPriority);

								// allocate vc to the challenger
								// change input fields
								m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = index;

								// change output fields
								m_northPort.m_inputPortRouted.at(index) = PortType::TerminalPort;
								m_northPort.m_inputVirtualChannelAllocated.at(index) = 0; // no virtual channel for ternimal port
								m_northPort.m_inputVirtualChannelPriority.at(index) = m_terminalPort.m_virtualChannel.m_virtualChannelPriority;
								goto allocateTerminalToNorthDone;
							}
						}
					}
				allocateTerminalToNorthDone: ; // goto statement
				}

				// if terminal port is routed to south port
				if (m_terminalPort.m_virtualChannel.m_outputPortRouted == PortType::SouthPort)
				{
					for (int i{}; i < VC_NUMBER; ++i)
					{
						if (m_southPort.m_downstreamVirtualChannelStates.at(i)
							== VirtualChannelState::I)
						{
							// insert an entry in m_virtualChannelArbitrationRecorder
							winVirtualChannelArbitration(PortType::TerminalPort, 0, m_terminalPort.m_virtualChannel.m_virtualChannelPriority);

							// change input fields
							m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = i;

							// change output fields
							m_southPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::V;
							m_southPort.m_inputPortRouted.at(i) = PortType::TerminalPort;
							m_southPort.m_inputVirtualChannelAllocated.at(i) = 0; // no virtual channel for ternimal port
							m_southPort.m_inputVirtualChannelPriority.at(i) = m_terminalPort.m_virtualChannel.m_virtualChannelPriority;
							goto allocateTerminalToSouthDone;
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
							if (m_terminalPort.m_virtualChannel.m_virtualChannelPriority < priority)
							{
								// find loser's record and delete it in m_virtualChannelArbitrationRecorder
								loseVirtualChannelArbitration(m_southPort.m_inputPortRouted.at(index), m_southPort.m_inputVirtualChannelAllocated.at(index), m_southPort.m_inputVirtualChannelPriority.at(index));

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

								// record winner in arbitration recorder
								winVirtualChannelArbitration(PortType::TerminalPort, 0, m_terminalPort.m_virtualChannel.m_virtualChannelPriority);

								// allocate vc to the challenger
								// change input fields
								m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = index;

								// change output fields
								m_southPort.m_inputPortRouted.at(index) = PortType::TerminalPort;
								m_southPort.m_inputVirtualChannelAllocated.at(index) = 0; // no virtual channel for ternimal port
								m_southPort.m_inputVirtualChannelPriority.at(index) = m_terminalPort.m_virtualChannel.m_virtualChannelPriority;
								goto allocateTerminalToSouthDone;
							}
						}
					}
				allocateTerminalToSouthDone: ;
				}

				// if terminal port is routed to west port
				if (m_terminalPort.m_virtualChannel.m_outputPortRouted == PortType::WestPort)
				{
					for (int i{}; i < VC_NUMBER; ++i)
					{
						if (m_westPort.m_downstreamVirtualChannelStates.at(i)
							== VirtualChannelState::I)
						{
							// insert an entry in m_virtualChannelArbitrationRecorder
							winVirtualChannelArbitration(PortType::TerminalPort, 0, m_terminalPort.m_virtualChannel.m_virtualChannelPriority);

							// change input fields
							m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = i;

							// change output fields
							m_westPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::V;
							m_westPort.m_inputPortRouted.at(i) = PortType::TerminalPort;
							m_westPort.m_inputVirtualChannelAllocated.at(i) = 0; // no virtual channel for ternimal port
							m_westPort.m_inputVirtualChannelPriority.at(i) = m_terminalPort.m_virtualChannel.m_virtualChannelPriority;
							goto allocateTerminalToWestDone;
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
							if (m_terminalPort.m_virtualChannel.m_virtualChannelPriority < priority)
							{
								// find loser's record and delete it in m_virtualChannelArbitrationRecorder
								loseVirtualChannelArbitration(m_westPort.m_inputPortRouted.at(index), m_westPort.m_inputVirtualChannelAllocated.at(index), m_westPort.m_inputVirtualChannelPriority.at(index));
						
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

								// record winner in arbitration recorder
								winVirtualChannelArbitration(PortType::TerminalPort, 0, m_terminalPort.m_virtualChannel.m_virtualChannelPriority);

								// allocate vc to the challenger
								// change input fields
								m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = index;

								// change output fields
								m_westPort.m_inputPortRouted.at(index) = PortType::TerminalPort;
								m_westPort.m_inputVirtualChannelAllocated.at(index) = 0; // no virtual channel for ternimal port
								m_westPort.m_inputVirtualChannelPriority.at(index) = m_terminalPort.m_virtualChannel.m_virtualChannelPriority;
								goto allocateTerminalToWestDone;
							}
						}
					}
				allocateTerminalToWestDone: ;
				}

				// if terminal port is routed to east port
				if (m_terminalPort.m_virtualChannel.m_outputPortRouted == PortType::EastPort)
				{
					for (int i{}; i < VC_NUMBER; ++i)
					{
						if (m_eastPort.m_downstreamVirtualChannelStates.at(i)
							== VirtualChannelState::I)
						{
							// insert an entry in m_virtualChannelArbitrationRecorder
							winVirtualChannelArbitration(PortType::TerminalPort, 0, m_terminalPort.m_virtualChannel.m_virtualChannelPriority);
					
							// change input fields
							m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = i;

							// change output fields
							m_eastPort.m_downstreamVirtualChannelStates.at(i) = VirtualChannelState::V;
							m_eastPort.m_inputPortRouted.at(i) = PortType::TerminalPort;
							m_eastPort.m_inputVirtualChannelAllocated.at(i) = 0; // no virtual channel for ternimal port
							m_eastPort.m_inputVirtualChannelPriority.at(i) = m_terminalPort.m_virtualChannel.m_virtualChannelPriority;
							goto allocateTerminalToEastDone;
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
							if (m_terminalPort.m_virtualChannel.m_virtualChannelPriority < priority)
							{
								// find loser's record and delete it in m_virtualChannelArbitrationRecorder
								loseVirtualChannelArbitration(m_eastPort.m_inputPortRouted.at(index), m_eastPort.m_inputVirtualChannelAllocated.at(index), m_eastPort.m_inputVirtualChannelPriority.at(index));

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

								// record winner in arbitration recorder
								winVirtualChannelArbitration(PortType::TerminalPort, 0, m_terminalPort.m_virtualChannel.m_virtualChannelPriority);

								// allocate vc to the challenger
								// change input fields
								m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = index;

								// change output fields
								m_eastPort.m_inputPortRouted.at(index) = PortType::TerminalPort;
								m_eastPort.m_inputVirtualChannelAllocated.at(index) = 0; // no virtual channel for ternimal port
								m_eastPort.m_inputVirtualChannelPriority.at(index) = m_terminalPort.m_virtualChannel.m_virtualChannelPriority;
								goto allocateTerminalToEastDone;
							}
						}
					}
				allocateTerminalToEastDone: ;
				}
				
				m_terminalPort.m_localClock.tickTriggerClock(PERIOD_ROUTER_VA - EXECUTION_TIME_ROUTER_VA + 1);
				m_terminalPort.m_localClock.toggleWaitingForExecution();
			}
		}
	}
}

void Router::allocateNorthPortVirtualChannel()
{
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_northPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_northPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V)
			{
				if (!m_northPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_northPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_VA - 1);
					m_northPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_northPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					// if north port is routed to terminal port
					if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
					{
						if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::I)
						{
							// insert an entry in m_virtualChannelArbitrationRecorder
							winVirtualChannelArbitration(PortType::NorthPort, i, m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority);

							// change input fields
							m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

							// change output fields
							m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::V;
							m_terminalPort.m_inputPortRouted = PortType::NorthPort;
							m_terminalPort.m_inputVirtualChannelAllocated = i;
							m_terminalPort.m_inputVirtualChannelPriority = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto allocateNorthToTerminalDone;
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
								// find loser's record and delete it in m_virtualChannelArbitrationRecorder
								loseVirtualChannelArbitration(m_terminalPort.m_inputPortRouted, m_terminalPort.m_inputVirtualChannelAllocated, m_terminalPort.m_inputVirtualChannelPriority);

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

								// record winner in arbitration recorder
								winVirtualChannelArbitration(PortType::NorthPort, i, m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority);

								// allocate vc to the challenger
								// change input fields
								m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

								// change output fields
								m_terminalPort.m_inputPortRouted = PortType::NorthPort;
								m_terminalPort.m_inputVirtualChannelAllocated = i;
								m_terminalPort.m_inputVirtualChannelPriority = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							}
							goto allocateNorthToTerminalDone;
						}
					allocateNorthToTerminalDone: ;
					}

					// if north port is routed to south port
					if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_southPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::NorthPort, i, m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_southPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_southPort.m_inputPortRouted.at(j) = PortType::NorthPort;
								m_southPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_southPort.m_inputVirtualChannelPriority.at(j) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateNorthToSouthDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_southPort.m_inputPortRouted.at(index), m_southPort.m_inputVirtualChannelAllocated.at(index), m_southPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_southPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::NorthPort, i, m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority);

									// allocate vc to the challenger
									// change input fields
									m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_southPort.m_inputPortRouted.at(index) = PortType::NorthPort;
									m_southPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_southPort.m_inputVirtualChannelPriority.at(index) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateNorthToSouthDone;
								}
							}
						}
					allocateNorthToSouthDone: ;
					}

					// if north port is routed to west port
					if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_westPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::NorthPort, i, m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_westPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_westPort.m_inputPortRouted.at(j) = PortType::NorthPort;
								m_westPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_westPort.m_inputVirtualChannelPriority.at(j) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateNorthToWestDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_westPort.m_inputPortRouted.at(index), m_westPort.m_inputVirtualChannelAllocated.at(index), m_westPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_westPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::NorthPort, i, m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_westPort.m_inputPortRouted.at(index) = PortType::NorthPort;
									m_westPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_westPort.m_inputVirtualChannelPriority.at(index) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateNorthToWestDone;
								}
							}
						}
					allocateNorthToWestDone: ;
					}

					// if north port is routed to east port
					if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::NorthPort, i, m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority);

								// change input fields
								m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_eastPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_eastPort.m_inputPortRouted.at(j) = PortType::NorthPort;
								m_eastPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_eastPort.m_inputVirtualChannelPriority.at(j) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateNorthToEastDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_eastPort.m_inputPortRouted.at(index), m_eastPort.m_inputVirtualChannelAllocated.at(index), m_eastPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::NorthPort, i, m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority);

									// allocate vc to the challenger
									// change input fields
									m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_eastPort.m_inputPortRouted.at(index) = PortType::NorthPort;
									m_eastPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_eastPort.m_inputVirtualChannelPriority.at(index) = m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateNorthToEastDone;
								}
							}
						}
					allocateNorthToEastDone: ;
					}

					m_northPort.m_virtualChannels.at(i).m_localClock.tickTriggerClock(PERIOD_ROUTER_VA - EXECUTION_TIME_ROUTER_VA + 1);
					m_northPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::allocateSouthPortVirtualChannel()
{
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_southPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_southPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V)
			{
				if (!m_southPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_southPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_VA - 1);
					m_southPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_southPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					// if south port is routed to terminal port
					if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
					{
						if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::I)
						{
							// insert an entry in m_virtualChannelArbitrationRecorder
							winVirtualChannelArbitration(PortType::SouthPort, i, m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority);
					
							// change input fields
							m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

							// change output fields
							m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::V;
							m_terminalPort.m_inputPortRouted = PortType::SouthPort;
							m_terminalPort.m_inputVirtualChannelAllocated = i;
							m_terminalPort.m_inputVirtualChannelPriority = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto allocateSouthToTerminalDone;
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
								// find loser's record and delete it in m_virtualChannelArbitrationRecorder
								loseVirtualChannelArbitration(m_terminalPort.m_inputPortRouted, m_terminalPort.m_inputVirtualChannelAllocated, m_terminalPort.m_inputVirtualChannelPriority);

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
						
								// record winner in arbitration recorder
								winVirtualChannelArbitration(PortType::SouthPort, i, m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority);

								// allocate vc to the challenger
								// change input fields
								m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

								// change output fields
								m_terminalPort.m_inputPortRouted = PortType::SouthPort;
								m_terminalPort.m_inputVirtualChannelAllocated = i;
								m_terminalPort.m_inputVirtualChannelPriority = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateSouthToTerminalDone;
							}
						}
					allocateSouthToTerminalDone: ;
					}

					// if south port is routed to north port
					if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_northPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::SouthPort, i, m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_northPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_northPort.m_inputPortRouted.at(j) = PortType::SouthPort;
								m_northPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_northPort.m_inputVirtualChannelPriority.at(j) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateSouthToNorthDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_northPort.m_inputPortRouted.at(index), m_northPort.m_inputVirtualChannelAllocated.at(index), m_northPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_northPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::SouthPort, i, m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_northPort.m_inputPortRouted.at(index) = PortType::SouthPort;
									m_northPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_northPort.m_inputVirtualChannelPriority.at(index) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateSouthToNorthDone;
								}
							}
						}
					allocateSouthToNorthDone: ;
					}

					// if south port is routed to west port
					if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_westPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::SouthPort, i, m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_westPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_westPort.m_inputPortRouted.at(j) = PortType::SouthPort;
								m_westPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_westPort.m_inputVirtualChannelPriority.at(j) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateSouthToWestDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_westPort.m_inputPortRouted.at(index), m_westPort.m_inputVirtualChannelAllocated.at(index), m_westPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_westPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::SouthPort, i, m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_westPort.m_inputPortRouted.at(index) = PortType::SouthPort;
									m_westPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_westPort.m_inputVirtualChannelPriority.at(index) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateSouthToWestDone;
								}
							}
						}
					allocateSouthToWestDone: ;
					}

					// if south port is routed to east port
					if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::SouthPort, i, m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_eastPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_eastPort.m_inputPortRouted.at(j) = PortType::SouthPort;
								m_eastPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_eastPort.m_inputVirtualChannelPriority.at(j) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateSouthToEastDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_eastPort.m_inputPortRouted.at(index), m_eastPort.m_inputVirtualChannelAllocated.at(index), m_eastPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::SouthPort, i, m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_eastPort.m_inputPortRouted.at(index) = PortType::SouthPort;
									m_eastPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_eastPort.m_inputVirtualChannelPriority.at(index) = m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateSouthToEastDone;
								}
							}
						}
					allocateSouthToEastDone: ;
					}

					m_southPort.m_virtualChannels.at(i).m_localClock.tickTriggerClock(PERIOD_ROUTER_VA - EXECUTION_TIME_ROUTER_VA + 1);
					m_southPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::allocateWestPortVirtualChannel()
{
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_westPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_westPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V)
			{
				if (!m_westPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_westPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_VA - 1);
					m_westPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_westPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					// if west port is routed to terminal port
					if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
					{
						if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::I)
						{
							// insert an entry in m_virtualChannelArbitrationRecorder
							winVirtualChannelArbitration(PortType::WestPort, i, m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority);
					
							// change input fields
							m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

							// change output fields
							m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::V;
							m_terminalPort.m_inputPortRouted = PortType::WestPort;
							m_terminalPort.m_inputVirtualChannelAllocated = i;
							m_terminalPort.m_inputVirtualChannelPriority = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto allocateWestToTerminalDone;
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
								// find loser's record and delete it in m_virtualChannelArbitrationRecorder
								loseVirtualChannelArbitration(m_terminalPort.m_inputPortRouted, m_terminalPort.m_inputVirtualChannelAllocated, m_terminalPort.m_inputVirtualChannelPriority);

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

								// record winner in arbitration recorder
								winVirtualChannelArbitration(PortType::WestPort, i, m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// allocate vc to the challenger
								// change input fields
								m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

								// change output fields
								m_terminalPort.m_inputPortRouted = PortType::WestPort;
								m_terminalPort.m_inputVirtualChannelAllocated = i;
								m_terminalPort.m_inputVirtualChannelPriority = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateWestToTerminalDone;
							}
						}
					allocateWestToTerminalDone: ;
					}

					// if west port is routed to north port
					if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_northPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::WestPort, i, m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_northPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_northPort.m_inputPortRouted.at(j) = PortType::WestPort;
								m_northPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_northPort.m_inputVirtualChannelPriority.at(j) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateWestToNorthDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_northPort.m_inputPortRouted.at(index), m_northPort.m_inputVirtualChannelAllocated.at(index), m_northPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_northPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::WestPort, i, m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_northPort.m_inputPortRouted.at(index) = PortType::WestPort;
									m_northPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_northPort.m_inputVirtualChannelPriority.at(index) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateWestToNorthDone;
								}
							}
						}
					allocateWestToNorthDone: ;
					}

					// if west port is routed to south port
					if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_southPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::WestPort, i, m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_southPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_southPort.m_inputPortRouted.at(j) = PortType::WestPort;
								m_southPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_southPort.m_inputVirtualChannelPriority.at(j) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateWestToSouthDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_southPort.m_inputPortRouted.at(index), m_southPort.m_inputVirtualChannelAllocated.at(index), m_southPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_southPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::WestPort, i, m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_southPort.m_inputPortRouted.at(index) = PortType::WestPort;
									m_southPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_southPort.m_inputVirtualChannelPriority.at(index) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateWestToSouthDone;
								}
							}
						}
					allocateWestToSouthDone: ;
					}

					// if west port is routed to east port
					if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_eastPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::WestPort, i, m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_eastPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_eastPort.m_inputPortRouted.at(j) = PortType::WestPort;
								m_eastPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_eastPort.m_inputVirtualChannelPriority.at(j) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateWestToEastDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_eastPort.m_inputPortRouted.at(index), m_eastPort.m_inputVirtualChannelAllocated.at(index), m_eastPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_eastPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_eastPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::WestPort, i, m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_eastPort.m_inputPortRouted.at(index) = PortType::WestPort;
									m_eastPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_eastPort.m_inputVirtualChannelPriority.at(index) = m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateWestToEastDone;
								}
							}
						}
					allocateWestToEastDone: ;
					}

					m_westPort.m_virtualChannels.at(i).m_localClock.tickTriggerClock(PERIOD_ROUTER_VA - EXECUTION_TIME_ROUTER_VA + 1);
					m_westPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::allocateEastPortVirtualChannel()
{
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_eastPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::V)
			{
				if (!m_eastPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_eastPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_VA - 1);
					m_eastPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_eastPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					// if east port is routed to terminal port
					if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
					{
						if (m_terminalPort.m_downstreamVirtualChannelState == VirtualChannelState::I)
						{
							// insert an entry in m_virtualChannelArbitrationRecorder
							winVirtualChannelArbitration(PortType::EastPort, i, m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority);
					
							// change input fields
							m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

							// change output fields
							m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::V;
							m_terminalPort.m_inputPortRouted = PortType::EastPort;
							m_terminalPort.m_inputVirtualChannelAllocated = i;
							m_terminalPort.m_inputVirtualChannelPriority = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
							goto allocateEastToTerminalDone;
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
								// find loser's record and delete it in m_virtualChannelArbitrationRecorder
								loseVirtualChannelArbitration(m_terminalPort.m_inputPortRouted, m_terminalPort.m_inputVirtualChannelAllocated, m_terminalPort.m_inputVirtualChannelPriority);

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

								// record winner in arbitration recorder
								winVirtualChannelArbitration(PortType::EastPort, i, m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// allocate vc to the challenger
								// change input fields
								m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = 0; // there is no virtual channel in terminal port

								// change output fields
								m_terminalPort.m_inputPortRouted = PortType::EastPort;
								m_terminalPort.m_inputVirtualChannelAllocated = i;
								m_terminalPort.m_inputVirtualChannelPriority = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateEastToTerminalDone;
							}
						}
					allocateEastToTerminalDone: ;
					}

					// if east port is routed to north port
					if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_northPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::EastPort, i, m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_northPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_northPort.m_inputPortRouted.at(j) = PortType::EastPort;
								m_northPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_northPort.m_inputVirtualChannelPriority.at(j) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateEastToNorthDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_northPort.m_inputPortRouted.at(index), m_northPort.m_inputVirtualChannelAllocated.at(index), m_northPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_northPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_northPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_northPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::EastPort, i, m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_northPort.m_inputPortRouted.at(index) = PortType::EastPort;
									m_northPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_northPort.m_inputVirtualChannelPriority.at(index) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateEastToNorthDone;
								}
							}
						}
					allocateEastToNorthDone: ;
					}

					// if east port is routed to south port
					if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_southPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::EastPort, i, m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_southPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_southPort.m_inputPortRouted.at(j) = PortType::EastPort;
								m_southPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_southPort.m_inputVirtualChannelPriority.at(j) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateEastToSouthDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_southPort.m_inputPortRouted.at(index), m_southPort.m_inputVirtualChannelAllocated.at(index), m_southPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_southPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::WestPort)
										m_westPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_southPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_southPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::EastPort, i, m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_southPort.m_inputPortRouted.at(index) = PortType::EastPort;
									m_southPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_southPort.m_inputVirtualChannelPriority.at(index) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateEastToSouthDone;
								}
							}
						}
					allocateEastToSouthDone: ;
					}

					// if east port is routed to west port
					if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
					{
						for (int j{}; j < VC_NUMBER; ++j)
						{
							if (m_westPort.m_downstreamVirtualChannelStates.at(j)
								== VirtualChannelState::I)
							{
								// insert an entry in m_virtualChannelArbitrationRecorder
								winVirtualChannelArbitration(PortType::EastPort, i, m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority);
						
								// change input fields
								m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = j;

								// change output fields
								m_westPort.m_downstreamVirtualChannelStates.at(j) = VirtualChannelState::V;
								m_westPort.m_inputPortRouted.at(j) = PortType::EastPort;
								m_westPort.m_inputVirtualChannelAllocated.at(j) = i;
								m_westPort.m_inputVirtualChannelPriority.at(j) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
								goto allocateEastToWestDone;
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
									// find loser's record and delete it in m_virtualChannelArbitrationRecorder
									loseVirtualChannelArbitration(m_westPort.m_inputPortRouted.at(index), m_westPort.m_inputVirtualChannelAllocated.at(index), m_westPort.m_inputVirtualChannelPriority.at(index));

									// deallocate the loser's vc
									// note that output port cannot be routed to input port with the same type
									// there should be four cases
									if (m_westPort.m_inputPortRouted.at(index) == PortType::TerminalPort)
										m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::NorthPort)
										m_northPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::SouthPort)
										m_southPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;
									if (m_westPort.m_inputPortRouted.at(index) == PortType::EastPort)
										m_eastPort.m_virtualChannels.at(m_westPort.m_inputVirtualChannelAllocated.at(index)).m_outputVirtualChannelAllocated = -1;

									// record winner in arbitration recorder
									winVirtualChannelArbitration(PortType::EastPort, i, m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority);
							
									// allocate vc to the challenger
									// change input fields
									m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated = index;

									// change output fields
									m_westPort.m_inputPortRouted.at(index) = PortType::EastPort;
									m_westPort.m_inputVirtualChannelAllocated.at(index) = i;
									m_westPort.m_inputVirtualChannelPriority.at(index) = m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority;
									goto allocateEastToWestDone;
								}
							}
						}
					allocateEastToWestDone: ;
					}

					m_eastPort.m_virtualChannels.at(i).m_localClock.tickTriggerClock(PERIOD_ROUTER_VA - EXECUTION_TIME_ROUTER_VA + 1);
					m_eastPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::winVirtualChannelArbitration(const PortType port, const int virtualChannel, const int virtualChannelPriority)
{
	m_virtualChannelArbitrationRecorder.push_back({ port, virtualChannel, virtualChannelPriority });
}

void Router::loseVirtualChannelArbitration(const PortType port, const int virtualChannel, const int virtualChannelPriority)
{
	VirtualChannelArbitrationRecorderLine record {port, virtualChannel, virtualChannelPriority };
	std::erase(m_virtualChannelArbitrationRecorder, record); // C++20
	//m_virtualChannelArbitrationRecorder.erase(std::remove(m_virtualChannelArbitrationRecorder.begin(), m_virtualChannelArbitrationRecorder.end(), record), m_virtualChannelArbitrationRecorder.end());
}

void Router::activateVirtualChannel()
{
	// activate terminal port
	if (m_terminalPort.m_virtualChannel.m_virtualChannelState == VirtualChannelState::V
		&& m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated != -1)
		m_terminalPort.m_virtualChannel.m_virtualChannelState = VirtualChannelState::A;
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
	// update terminal port
	int counter{};
	for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
	{
		if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::TerminalPort
			&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == 0)
		{
			m_terminalPort.m_virtualChannel.m_virtualChannelPriority = (1 + VC_NUMBER * 4) - m_virtualChannelArbitrationRecorder.size() + j;
			goto updateTerminalVirtualChannelPriorityDone;
		}
	}
	for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
	{
		if (m_terminalPort.m_virtualChannel.m_virtualChannelPriority > m_virtualChannelArbitrationRecorder.at(j).virtualChannelPriority)
		{
			counter++;
		}
	}
	m_terminalPort.m_virtualChannel.m_virtualChannelPriority -= counter;
updateTerminalVirtualChannelPriorityDone:;

	// update north port
	for (int i{}; i < m_northPort.m_virtualChannels.size(); ++i)
	{
		counter = 0;
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::NorthPort 
				&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == i)
			{
				m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority = (1 + VC_NUMBER * 4) - m_virtualChannelArbitrationRecorder.size() + j;
				goto updateNorthVirtualChannelPriorityDone;
			}
		}
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority > m_virtualChannelArbitrationRecorder.at(j).virtualChannelPriority)
			{
				counter++;
			}
		}
		m_northPort.m_virtualChannels.at(i).m_virtualChannelPriority -= counter;
	updateNorthVirtualChannelPriorityDone:;
	}

	// update south port
	for (int i{}; i < m_southPort.m_virtualChannels.size(); ++i)
	{
		counter = 0;
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::SouthPort
				&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == i)
			{
				m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority = (1 + VC_NUMBER * 4) - m_virtualChannelArbitrationRecorder.size() + j;
				goto updateSouthVirtualChannelPriorityDone;
			}
		}
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority > m_virtualChannelArbitrationRecorder.at(j).virtualChannelPriority)
			{
				counter++;
			}
		}
		m_southPort.m_virtualChannels.at(i).m_virtualChannelPriority -= counter;
	updateSouthVirtualChannelPriorityDone:;
	}

	// update west port
	for (int i{}; i < m_westPort.m_virtualChannels.size(); ++i)
	{
		counter = 0;
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::WestPort
				&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == i)
			{
				m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority = (1 + VC_NUMBER * 4) - m_virtualChannelArbitrationRecorder.size() + j;
				goto updateWestVirtualChannelPriorityDone;
			}
		}
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority > m_virtualChannelArbitrationRecorder.at(j).virtualChannelPriority)
			{
				counter++;
			}
		}
		m_westPort.m_virtualChannels.at(i).m_virtualChannelPriority -= counter;
	updateWestVirtualChannelPriorityDone:;
	}

	// update east port
	for (int i{}; i < m_eastPort.m_virtualChannels.size(); ++i)
	{
		counter = 0;
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_virtualChannelArbitrationRecorder.at(j).port == PortType::EastPort
				&& m_virtualChannelArbitrationRecorder.at(j).virtualChannel == i)
			{
				m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority = (1 + VC_NUMBER * 4) - m_virtualChannelArbitrationRecorder.size() + j;
				goto updateEastVirtualChannelPriorityDone;
			}
		}
		for (int j{}; j < m_virtualChannelArbitrationRecorder.size(); ++j)
		{
			if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority > m_virtualChannelArbitrationRecorder.at(j).virtualChannelPriority)
			{
				counter++;
			}
		}
		m_eastPort.m_virtualChannels.at(i).m_virtualChannelPriority -= counter;
	updateEastVirtualChannelPriorityDone:;
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
	receiveFlitAndCredit();

	// allocate terminal port
	allocateTerminalPortSwitch();

	// allocate north port
	allocateNorthPortSwitch();

	// allocate south port
	allocateSouthPortSwitch();

	// allocate west port
	allocateWestPortSwitch();

	// allocate east port
	allocateEastPortSwitch();

	// arbitrate output port, terminate losers' connection
	switchArbitration();

	// update switch priority global and switch priority local
	updateSwitchPriority();

	// get one flit out, set fields in the flit, update credits
	getOneFlitOut();

}

void Router::allocateTerminalPortSwitch()
{
	if (m_terminalPort.m_localClock.triggerLocalEvent())
	{
		// allocate switch
		if (m_terminalPort.m_virtualChannel.m_virtualChannelState == VirtualChannelState::A)
		{
			if (!m_terminalPort.m_localClock.isWaitingForExecution())
			{
				m_terminalPort.m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
				m_terminalPort.m_localClock.toggleWaitingForExecution();
			}

			if (m_terminalPort.m_localClock.executeLocalEvent())
			{
				// if terminal port is routed to north port 
				if (m_terminalPort.m_virtualChannel.m_outputPortRouted == PortType::NorthPort)
				{
					if (m_northPort.m_credit.at(m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated) > 0)
						m_terminalPort.m_outputPortSwitched = PortType::NorthPort;
				}

				// if terminal port is routed to south port 
				if (m_terminalPort.m_virtualChannel.m_outputPortRouted == PortType::SouthPort)
				{
					if (m_southPort.m_credit.at(m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated) > 0)
						m_terminalPort.m_outputPortSwitched = PortType::SouthPort;
				}

				// if terminal port is routed to west port 
				if (m_terminalPort.m_virtualChannel.m_outputPortRouted == PortType::WestPort)
				{
					if (m_westPort.m_credit.at(m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated) > 0)
						m_terminalPort.m_outputPortSwitched = PortType::WestPort;
				}

				// if terminal port is routed to east port 
				if (m_terminalPort.m_virtualChannel.m_outputPortRouted == PortType::EastPort)
				{
					if (m_eastPort.m_credit.at(m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated) > 0)
						m_terminalPort.m_outputPortSwitched = PortType::EastPort;
				}

				m_terminalPort.m_localClock.tickTriggerClock(PERIOD_ROUTER_SA - EXECUTION_TIME_ROUTER_SA + 1);
				m_terminalPort.m_localClock.toggleWaitingForExecution();
			}
		}
	}

}

void Router::allocateNorthPortSwitch()
{
	// find vc in state A with the highest local priority
	bool isFirstActiveVirtualChannel{ true };
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_northPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_northPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::A)
			{
				if (!m_northPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_northPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
					m_northPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_northPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					// if north port is routed to terminal port 
					if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
					{
						if (isFirstActiveVirtualChannel)
						{
							m_northPort.m_virtualChannelSwitched = i;
							isFirstActiveVirtualChannel = false;
						}
						else if (!isFirstActiveVirtualChannel
							&& m_northPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_switchPriorityLocal)
						{
							m_northPort.m_virtualChannelSwitched = i;
						}
					}

					// if north port is routed to south port 
					if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
					{
						if (m_southPort.m_credit.at(m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_northPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_northPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_northPort.m_virtualChannelSwitched = i;
							}
						}
					}

					// if north port is routed to west port 
					if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
					{
						if (m_westPort.m_credit.at(m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_northPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_northPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_northPort.m_virtualChannelSwitched = i;
							}
						}
					}

					// if north port is routed to east port 
					if (m_northPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
					{
						if (m_eastPort.m_credit.at(m_northPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_northPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_northPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_northPort.m_virtualChannelSwitched = i;
							}
						}
					}

					m_northPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}

	// allocate switch
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_northPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_northPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::A)
			{
				if (!m_northPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_northPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
					m_northPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_northPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					m_northPort.m_outputPortSwitched = m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_outputPortRouted;
					m_northPort.m_virtualChannels.at(i).m_localClock.tickTriggerClock(PERIOD_ROUTER_SA - EXECUTION_TIME_ROUTER_SA + 1);
					m_northPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::allocateSouthPortSwitch()
{
	// find vc in state A with the highest local priority
	bool isFirstActiveVirtualChannel{ true };
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_southPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_southPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::A)
			{
				if (!m_southPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_southPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
					m_southPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_southPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					// if south port is routed to terminal port 
					if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
					{
						if (isFirstActiveVirtualChannel)
						{
							m_southPort.m_virtualChannelSwitched = i;
							isFirstActiveVirtualChannel = false;
						}
						else if (!isFirstActiveVirtualChannel
							&& m_southPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_switchPriorityLocal)
						{
							m_southPort.m_virtualChannelSwitched = i;
						}
					}

					// if south port is routed to north port 
					if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
					{
						if (m_northPort.m_credit.at(m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_southPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_southPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_southPort.m_virtualChannelSwitched = i;
							}
						}
					}

					// if south port is routed to west port 
					if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
					{
						if (m_westPort.m_credit.at(m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_southPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_southPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_southPort.m_virtualChannelSwitched = i;
							}
						}
					}

					// if south port is routed to east port 
					if (m_southPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
					{
						if (m_eastPort.m_credit.at(m_southPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_southPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_southPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_southPort.m_virtualChannelSwitched = i;
							}
						}
					}

					m_southPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}

	// allocate switch
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_southPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_southPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::A)
			{
				if (!m_northPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_northPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
					m_northPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_northPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					m_southPort.m_outputPortSwitched = m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_outputPortRouted;
					m_northPort.m_virtualChannels.at(i).m_localClock.tickTriggerClock(PERIOD_ROUTER_SA - EXECUTION_TIME_ROUTER_SA + 1);
					m_southPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::allocateWestPortSwitch()
{
	// find vc in state A with the highest local priority
	bool isFirstActiveVirtualChannel{ true };
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_westPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_westPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::A)
			{
				if (!m_westPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_westPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
					m_westPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_westPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					// if west port is routed to terminal port 
					if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
					{
						if (isFirstActiveVirtualChannel)
						{
							m_westPort.m_virtualChannelSwitched = i;
							isFirstActiveVirtualChannel = false;
						}
						else if (!isFirstActiveVirtualChannel
							&& m_westPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_switchPriorityLocal)
						{
							m_westPort.m_virtualChannelSwitched = i;
						}
					}

					// if west port is routed to north port 
					if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
					{
						if (m_northPort.m_credit.at(m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_westPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_westPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_westPort.m_virtualChannelSwitched = i;
							}
						}
					}

					// if west port is routed to south port 
					if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
					{
						if (m_southPort.m_credit.at(m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_westPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_westPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_westPort.m_virtualChannelSwitched = i;
							}
						}
					}

					// if west port is routed to east port 
					if (m_westPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::EastPort)
					{
						if (m_eastPort.m_credit.at(m_westPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_westPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_westPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_westPort.m_virtualChannelSwitched = i;
							}
						}
					}

					m_westPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}

	// allocate switch
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_westPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_westPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::A)
			{
				if (!m_westPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_westPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
					m_westPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_westPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					m_westPort.m_outputPortSwitched = m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_outputPortRouted;
					m_westPort.m_virtualChannels.at(i).m_localClock.tickTriggerClock(PERIOD_ROUTER_SA - EXECUTION_TIME_ROUTER_SA + 1);
					m_westPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::allocateEastPortSwitch()
{
	// find vc in state A with the highest local priority
	bool isFirstActiveVirtualChannel{ true };
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_eastPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::A)
			{
				if (!m_eastPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_eastPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
					m_eastPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_eastPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					// if east port is routed to terminal port 
					if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::TerminalPort)
					{
						if (isFirstActiveVirtualChannel)
						{
							m_eastPort.m_virtualChannelSwitched = i;
							isFirstActiveVirtualChannel = false;
						}
						else if (!isFirstActiveVirtualChannel
							&& m_eastPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_switchPriorityLocal)
						{
							m_eastPort.m_virtualChannelSwitched = i;
						}
					}

					// if east port is routed to north port 
					if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::NorthPort)
					{
						if (m_northPort.m_credit.at(m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_eastPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_eastPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_eastPort.m_virtualChannelSwitched = i;
							}
						}
					}

					// if east port is routed to south port 
					if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::SouthPort)
					{
						if (m_southPort.m_credit.at(m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_eastPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_eastPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_eastPort.m_virtualChannelSwitched = i;
							}
						}
					}

					// if east port is routed to west port 
					if (m_eastPort.m_virtualChannels.at(i).m_outputPortRouted == PortType::WestPort)
					{
						if (m_westPort.m_credit.at(m_eastPort.m_virtualChannels.at(i).m_outputVirtualChannelAllocated) > 0)
						{
							if (isFirstActiveVirtualChannel)
							{
								m_eastPort.m_virtualChannelSwitched = i;
								isFirstActiveVirtualChannel = false;
							}
							else if (!isFirstActiveVirtualChannel
								&& m_eastPort.m_virtualChannels.at(i).m_switchPriorityLocal < m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_switchPriorityLocal)
							{
								m_eastPort.m_virtualChannelSwitched = i;
							}
						}
					}

					m_eastPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}

	// allocate switch
	for (int i{}; i < VC_NUMBER; ++i)
	{
		if (m_eastPort.m_virtualChannels.at(i).m_localClock.triggerLocalEvent())
		{
			if (m_eastPort.m_virtualChannels.at(i).m_virtualChannelState == VirtualChannelState::A)
			{
				if (!m_eastPort.m_virtualChannels.at(i).m_localClock.isWaitingForExecution())
				{
					m_eastPort.m_virtualChannels.at(i).m_localClock.tickExecutionClock(EXECUTION_TIME_ROUTER_SA - 1);
					m_eastPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}

				if (m_eastPort.m_virtualChannels.at(i).m_localClock.executeLocalEvent())
				{
					m_eastPort.m_outputPortSwitched = m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_outputPortRouted;
					m_eastPort.m_virtualChannels.at(i).m_localClock.tickTriggerClock(PERIOD_ROUTER_SA - EXECUTION_TIME_ROUTER_SA + 1);
					m_eastPort.m_virtualChannels.at(i).m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
}

void Router::switchArbitration()
{
	// arbitrate terminal port
	if (m_terminalPort.m_outputPortSwitched != PortType::Unselected)
	{
		winSwitchArbitration(PortType::TerminalPort, m_terminalPort.m_outputPortSwitched, m_terminalPort.m_switchPriorityGlobal);
	}
	
	// arbitrate north port
	if (m_northPort.m_outputPortSwitched != PortType::Unselected)
	{
		winSwitchArbitration(PortType::NorthPort, m_northPort.m_outputPortSwitched, m_northPort.m_switchPriorityGlobal);
		std::vector<SwitchArbitrationRecorderLine> t_switchArbitrationRecorder{ m_switchArbitrationRecorder };
		for (auto& record : t_switchArbitrationRecorder)
		{
			if (record.outputPortSwitched == m_northPort.m_outputPortSwitched)
			{
				if (record.switchPriorityGlobal > m_northPort.m_switchPriorityGlobal)
				{
					loseSwitchArbitration(record.port, record.outputPortSwitched, record.switchPriorityGlobal);
					if (record.port == PortType::TerminalPort)
					{
						m_terminalPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::SouthPort)
					{
						m_southPort.m_virtualChannelSwitched = 0;
						m_southPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::WestPort)
					{
						m_westPort.m_virtualChannelSwitched = 0;
						m_westPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::EastPort)
					{
						m_eastPort.m_virtualChannelSwitched = 0;
						m_eastPort.m_outputPortSwitched = PortType::Unselected;
					}
				}
				else if (record.switchPriorityGlobal < m_northPort.m_switchPriorityGlobal)
				{
					loseSwitchArbitration(PortType::NorthPort, m_northPort.m_outputPortSwitched, m_northPort.m_switchPriorityGlobal);
					m_northPort.m_virtualChannelSwitched = 0;
					m_northPort.m_outputPortSwitched = PortType::Unselected;
				}
			}
		}
	}
	
	// arbitrate south port
	if (m_southPort.m_outputPortSwitched != PortType::Unselected)
	{
		winSwitchArbitration(PortType::SouthPort, m_southPort.m_outputPortSwitched, m_southPort.m_switchPriorityGlobal);
		std::vector<SwitchArbitrationRecorderLine> t_switchArbitrationRecorder{ m_switchArbitrationRecorder };
		for (auto& record : t_switchArbitrationRecorder)
		{
			if (record.outputPortSwitched == m_southPort.m_outputPortSwitched)
			{
				if (record.switchPriorityGlobal > m_southPort.m_switchPriorityGlobal)
				{
					loseSwitchArbitration(record.port, record.outputPortSwitched, record.switchPriorityGlobal);
					if (record.port == PortType::TerminalPort)
					{
						m_terminalPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::NorthPort)
					{
						m_northPort.m_virtualChannelSwitched = 0;
						m_northPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::WestPort)
					{
						m_westPort.m_virtualChannelSwitched = 0;
						m_westPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::EastPort)
					{
						m_eastPort.m_virtualChannelSwitched = 0;
						m_eastPort.m_outputPortSwitched = PortType::Unselected;
					}
				}
				else if (record.switchPriorityGlobal < m_southPort.m_switchPriorityGlobal)
				{
					loseSwitchArbitration(PortType::SouthPort, m_southPort.m_outputPortSwitched, m_southPort.m_switchPriorityGlobal);
					m_southPort.m_virtualChannelSwitched = 0;
					m_southPort.m_outputPortSwitched = PortType::Unselected;
				}
			}
		}
	}
	
	// arbitrate west port
	if (m_westPort.m_outputPortSwitched != PortType::Unselected)
	{
		winSwitchArbitration(PortType::WestPort, m_westPort.m_outputPortSwitched, m_westPort.m_switchPriorityGlobal);
		std::vector<SwitchArbitrationRecorderLine> t_switchArbitrationRecorder{ m_switchArbitrationRecorder };
		for (auto& record : t_switchArbitrationRecorder)
		{
			if (record.outputPortSwitched == m_westPort.m_outputPortSwitched)
			{
				if (record.switchPriorityGlobal > m_westPort.m_switchPriorityGlobal)
				{
					loseSwitchArbitration(record.port, record.outputPortSwitched, record.switchPriorityGlobal);
					if (record.port == PortType::TerminalPort)
					{
						m_terminalPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::NorthPort)
					{
						m_northPort.m_virtualChannelSwitched = 0;
						m_northPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::SouthPort)
					{
						m_southPort.m_virtualChannelSwitched = 0;
						m_southPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::EastPort)
					{
						m_eastPort.m_virtualChannelSwitched = 0;
						m_eastPort.m_outputPortSwitched = PortType::Unselected;
					}
				}
				else if (record.switchPriorityGlobal < m_westPort.m_switchPriorityGlobal)
				{
					loseSwitchArbitration(PortType::WestPort, m_westPort.m_outputPortSwitched, m_westPort.m_switchPriorityGlobal);
					m_westPort.m_virtualChannelSwitched = 0;
					m_westPort.m_outputPortSwitched = PortType::Unselected;
				}
			}
		}
	}
	
	// arbitrate east port
	if (m_eastPort.m_outputPortSwitched != PortType::Unselected)
	{
		winSwitchArbitration(PortType::EastPort, m_eastPort.m_outputPortSwitched, m_eastPort.m_switchPriorityGlobal);
		std::vector<SwitchArbitrationRecorderLine> t_switchArbitrationRecorder{ m_switchArbitrationRecorder };
		for (auto& record : t_switchArbitrationRecorder)
		{
			if (record.outputPortSwitched == m_eastPort.m_outputPortSwitched)
			{
				if (record.switchPriorityGlobal > m_eastPort.m_switchPriorityGlobal)
				{
					loseSwitchArbitration(record.port, record.outputPortSwitched, record.switchPriorityGlobal);
					if (record.port == PortType::TerminalPort)
					{
						m_terminalPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::NorthPort)
					{
						m_northPort.m_virtualChannelSwitched = 0;
						m_northPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::SouthPort)
					{
						m_southPort.m_virtualChannelSwitched = 0;
						m_southPort.m_outputPortSwitched = PortType::Unselected;
					}
					if (record.port == PortType::WestPort)
					{
						m_westPort.m_virtualChannelSwitched = 0;
						m_westPort.m_outputPortSwitched = PortType::Unselected;
					}
				}
				else if (record.switchPriorityGlobal < m_eastPort.m_switchPriorityGlobal)
				{
					loseSwitchArbitration(PortType::EastPort, m_eastPort.m_outputPortSwitched, m_eastPort.m_switchPriorityGlobal);
					m_eastPort.m_virtualChannelSwitched = 0;
					m_eastPort.m_outputPortSwitched = PortType::Unselected;
				}
			}
		}
	}
}

void Router::winSwitchArbitration(const PortType port, const PortType outputPortSwitched, const int switchPriorityGlobal)
{
	m_switchArbitrationRecorder.push_back({port, outputPortSwitched, switchPriorityGlobal});
}

void Router::loseSwitchArbitration(const PortType port, const PortType outputPortSwitched, const int switchPriorityGlobal)
{
	SwitchArbitrationRecorderLine record{ port, outputPortSwitched, switchPriorityGlobal };
	std::erase(m_switchArbitrationRecorder, record); // C++20
	//m_switchArbitrationRecorder.erase(std::remove(m_switchArbitrationRecorder.begin(), m_switchArbitrationRecorder.end(), record), m_switchArbitrationRecorder.end());
}

void Router::updateSwitchPriority()
{
	// update terminal switch priority global
	int counter{};
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_switchArbitrationRecorder.at(i).port == m_terminalPort.m_portType)
		{
			m_terminalPort.m_switchPriorityGlobal = 5 - m_switchArbitrationRecorder.size() + i;
			goto updateTerminalSwitchPriorityGlobalDone;
		}
	}
	// if terminal port does not exist in switch arbitration recorder
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_terminalPort.m_switchPriorityGlobal > m_switchArbitrationRecorder.at(i).switchPriorityGlobal)
		{
			counter++;
		}
	}
	m_terminalPort.m_switchPriorityGlobal -= counter;
updateTerminalSwitchPriorityGlobalDone:;

	// update north switch priority global
	counter = 0;
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_switchArbitrationRecorder.at(i).port == m_northPort.m_portType)
		{
			m_northPort.m_switchPriorityGlobal = 5 - m_switchArbitrationRecorder.size() + i;
			goto updateNorthSwitchPriorityGlobalDone;
		}
	}
	// if north port does not exist in switch arbitration recorder
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_northPort.m_switchPriorityGlobal > m_switchArbitrationRecorder.at(i).switchPriorityGlobal)
		{
			counter++;
		}
	}
	m_northPort.m_switchPriorityGlobal -= counter;
updateNorthSwitchPriorityGlobalDone:;
	
	// update north switch priority local
	if (m_northPort.m_outputPortSwitched != PortType::Unselected)
	{
		for (int i{}; i < VC_NUMBER; ++i)
		{
			if (m_northPort.m_virtualChannels.at(i).m_switchPriorityLocal
				> m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_switchPriorityLocal)
			{
				m_northPort.m_virtualChannels.at(i).m_switchPriorityLocal--;
			}
		}
		m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_switchPriorityLocal = VC_NUMBER - 1;
	}

	// update south switch priority global
	counter = 0;
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_switchArbitrationRecorder.at(i).port == m_southPort.m_portType)
		{
			m_southPort.m_switchPriorityGlobal = 5 - m_switchArbitrationRecorder.size() + i;
			goto updateSouthSwitchPriorityGlobalDone;
		}
	}
	// if south port does not exist in switch arbitration recorder
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_southPort.m_switchPriorityGlobal > m_switchArbitrationRecorder.at(i).switchPriorityGlobal)
		{
			counter++;
		}
	}
	m_southPort.m_switchPriorityGlobal -= counter;
updateSouthSwitchPriorityGlobalDone:;

	// update south switch priority local
	if (m_southPort.m_outputPortSwitched != PortType::Unselected)
	{
		for (int i{}; i < VC_NUMBER; ++i)
		{
			if (m_southPort.m_virtualChannels.at(i).m_switchPriorityLocal
				> m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_switchPriorityLocal)
			{
				m_southPort.m_virtualChannels.at(i).m_switchPriorityLocal--;
			}
		}
		m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_switchPriorityLocal = VC_NUMBER - 1;
	}

	// update west switch priority global
	counter = 0;
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_switchArbitrationRecorder.at(i).port == m_westPort.m_portType)
		{
			m_westPort.m_switchPriorityGlobal = 5 - m_switchArbitrationRecorder.size() + i;
			goto updateWestSwitchPriorityGlobalDone;
		}
	}
	// if west port does not exist in switch arbitration recorder
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_westPort.m_switchPriorityGlobal > m_switchArbitrationRecorder.at(i).switchPriorityGlobal)
		{
			counter++;
		}
	}
	m_westPort.m_switchPriorityGlobal -= counter;
updateWestSwitchPriorityGlobalDone:;

	// update west switch priority local
	if (m_westPort.m_outputPortSwitched != PortType::Unselected)
	{
		for (int i{}; i < VC_NUMBER; ++i)
		{
			if (m_westPort.m_virtualChannels.at(i).m_switchPriorityLocal
				> m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_switchPriorityLocal)
			{
				m_westPort.m_virtualChannels.at(i).m_switchPriorityLocal--;
			}
		}
		m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_switchPriorityLocal = VC_NUMBER - 1;
	}

	// update east switch priority global
	counter = 0;
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_switchArbitrationRecorder.at(i).port == m_eastPort.m_portType)
		{
			m_eastPort.m_switchPriorityGlobal = 5 - m_switchArbitrationRecorder.size() + i;
			goto updateEastSwitchPriorityGlobalDone;
		}
	}
	// if east port does not exist in switch arbitration recorder
	for (int i{}; i < m_switchArbitrationRecorder.size(); ++i)
	{
		if (m_eastPort.m_switchPriorityGlobal > m_switchArbitrationRecorder.at(i).switchPriorityGlobal)
		{
			counter++;
		}
	}
	m_eastPort.m_switchPriorityGlobal -= counter;
updateEastSwitchPriorityGlobalDone:;

	// update east switch priority local
	if (m_eastPort.m_outputPortSwitched != PortType::Unselected)
	{
		for (int i{}; i < VC_NUMBER; ++i)
		{
			if (m_eastPort.m_virtualChannels.at(i).m_switchPriorityLocal
				> m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_switchPriorityLocal)
			{
				m_eastPort.m_virtualChannels.at(i).m_switchPriorityLocal--;
			}
		}
		m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_switchPriorityLocal = VC_NUMBER - 1;
	}

	m_switchArbitrationRecorder.clear();
}

void Router::getOneFlitOut()
{
	// terminal port
	if (m_terminalPort.m_outputPortSwitched != PortType::Unselected)
	{
		if (!m_terminalPort.m_virtualChannel.isEmpty()
			&& m_terminalPort.m_crossbarInputRegister.size() < REGISTER_DEPTH)
		{
			// update credit
			if (m_terminalPort.m_outputPortSwitched == PortType::NorthPort)
				m_northPort.m_credit.at(m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated)--;
			if (m_terminalPort.m_outputPortSwitched == PortType::SouthPort)
				m_southPort.m_credit.at(m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated)--;
			if (m_terminalPort.m_outputPortSwitched == PortType::WestPort)
				m_westPort.m_credit.at(m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated)--;
			if (m_terminalPort.m_outputPortSwitched == PortType::EastPort)
				m_eastPort.m_credit.at(m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated)--;

			// get one flit out
			Flit flit{ m_terminalPort.m_virtualChannel.popfrontFlit()};
			flit.port = m_terminalPort.m_outputPortSwitched;
			flit.virtualChannel = m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated;
			m_terminalPort.m_crossbarInputRegister.push_back(flit);
		}
	}

	// north port
	if (m_northPort.m_outputPortSwitched != PortType::Unselected)
	{
		if (!m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).isEmpty()
			&& m_northPort.m_crossbarInputRegister.size() < REGISTER_DEPTH)
		{
			// update credit
			if (m_northPort.m_outputPortSwitched == PortType::SouthPort)
				m_southPort.m_credit.at(m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;
			if (m_northPort.m_outputPortSwitched == PortType::WestPort)
				m_westPort.m_credit.at(m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;
			if (m_northPort.m_outputPortSwitched == PortType::EastPort)
				m_eastPort.m_credit.at(m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;

			// get one flit out
			Flit flit{ m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).popfrontFlit() };
			flit.port = m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_outputPortRouted;
			flit.virtualChannel = m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated;
			m_northPort.m_crossbarInputRegister.push_back(flit);
		}
	}

	// south port
	if (m_southPort.m_outputPortSwitched != PortType::Unselected)
	{
		if (!m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).isEmpty()
			&& m_southPort.m_crossbarInputRegister.size() < REGISTER_DEPTH)
		{
			// update credit
			if (m_southPort.m_outputPortSwitched == PortType::NorthPort)
				m_northPort.m_credit.at(m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;
			if (m_southPort.m_outputPortSwitched == PortType::WestPort)
				m_westPort.m_credit.at(m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;
			if (m_southPort.m_outputPortSwitched == PortType::EastPort)
				m_eastPort.m_credit.at(m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;

			// get one flit out
			Flit flit{ m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).popfrontFlit() };
			flit.port = m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_outputPortRouted;
			flit.virtualChannel = m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated;
			m_southPort.m_crossbarInputRegister.push_back(flit);
		}
	}

	// west port
	if (m_westPort.m_outputPortSwitched != PortType::Unselected)
	{
		if (!m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).isEmpty()
			&& m_westPort.m_crossbarInputRegister.size() < REGISTER_DEPTH)
		{
			// update credit
			if (m_westPort.m_outputPortSwitched == PortType::NorthPort)
				m_northPort.m_credit.at(m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;
			if (m_westPort.m_outputPortSwitched == PortType::SouthPort)
				m_southPort.m_credit.at(m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;
			if (m_westPort.m_outputPortSwitched == PortType::EastPort)
				m_eastPort.m_credit.at(m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;

			// get one flit out
			Flit flit{ m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).popfrontFlit() };
			flit.port = m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_outputPortRouted;
			flit.virtualChannel = m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated;
			m_westPort.m_crossbarInputRegister.push_back(flit);
		}
	}

	// east port
	if (m_eastPort.m_outputPortSwitched != PortType::Unselected)
	{
		if (!m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).isEmpty()
			&& m_eastPort.m_crossbarInputRegister.size() < REGISTER_DEPTH)
		{
			// update credit
			if (m_eastPort.m_outputPortSwitched == PortType::NorthPort)
				m_northPort.m_credit.at(m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;
			if (m_eastPort.m_outputPortSwitched == PortType::SouthPort)
				m_southPort.m_credit.at(m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;
			if (m_eastPort.m_outputPortSwitched == PortType::WestPort)
				m_westPort.m_credit.at(m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated)--;

			// get one flit out
			Flit flit{ m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).popfrontFlit() };
			flit.port = m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_outputPortRouted;
			flit.virtualChannel = m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated;
			m_eastPort.m_crossbarInputRegister.push_back(flit);
		}
	}
}

void Router::traverseSwitch()
{
	// setup crossbar connections
	setUpCrossbarConnections();

	// crossbar traversal
	m_crossbar.traversal();

	// input port send credit to upstream
	transmitCredits();

	// reset fields
	resetFields();

	// crossbar terminate all connections
	m_crossbar.terminateAllConnections();
}

void Router::setUpCrossbarConnections()
{
	if (!m_terminalPort.m_crossbarInputRegister.empty())
	{
		if (m_terminalPort.m_outputPortSwitched == PortType::NorthPort)
			m_crossbar.setUpConnection(m_terminalPort, m_northPort);
		if (m_terminalPort.m_outputPortSwitched == PortType::SouthPort)
			m_crossbar.setUpConnection(m_terminalPort, m_southPort);
		if (m_terminalPort.m_outputPortSwitched == PortType::WestPort)
			m_crossbar.setUpConnection(m_terminalPort, m_westPort);
		if (m_terminalPort.m_outputPortSwitched == PortType::EastPort)
			m_crossbar.setUpConnection(m_terminalPort, m_eastPort);
	}

	if (!m_northPort.m_crossbarInputRegister.empty())
	{
		if (m_northPort.m_outputPortSwitched == PortType::TerminalPort)
			m_crossbar.setUpConnection(m_northPort, m_terminalPort);
		if (m_northPort.m_outputPortSwitched == PortType::SouthPort)
			m_crossbar.setUpConnection(m_northPort, m_southPort);
		if (m_northPort.m_outputPortSwitched == PortType::WestPort)
			m_crossbar.setUpConnection(m_northPort, m_westPort);
		if (m_northPort.m_outputPortSwitched == PortType::EastPort)
			m_crossbar.setUpConnection(m_northPort, m_eastPort);
	}

	if (!m_southPort.m_crossbarInputRegister.empty())
	{
		if (m_southPort.m_outputPortSwitched == PortType::TerminalPort)
			m_crossbar.setUpConnection(m_southPort, m_terminalPort);
		if (m_southPort.m_outputPortSwitched == PortType::NorthPort)
			m_crossbar.setUpConnection(m_southPort, m_northPort);
		if (m_southPort.m_outputPortSwitched == PortType::WestPort)
			m_crossbar.setUpConnection(m_southPort, m_westPort);
		if (m_southPort.m_outputPortSwitched == PortType::EastPort)
			m_crossbar.setUpConnection(m_southPort, m_eastPort);
	}

	if (!m_westPort.m_crossbarInputRegister.empty())
	{
		if (m_westPort.m_outputPortSwitched == PortType::TerminalPort)
			m_crossbar.setUpConnection(m_westPort, m_terminalPort);
		if (m_westPort.m_outputPortSwitched == PortType::NorthPort)
			m_crossbar.setUpConnection(m_westPort, m_northPort);
		if (m_westPort.m_outputPortSwitched == PortType::SouthPort)
			m_crossbar.setUpConnection(m_westPort, m_southPort);
		if (m_westPort.m_outputPortSwitched == PortType::EastPort)
			m_crossbar.setUpConnection(m_westPort, m_eastPort);
	}

	if (!m_eastPort.m_crossbarInputRegister.empty())
	{
		if (m_eastPort.m_outputPortSwitched == PortType::TerminalPort)
			m_crossbar.setUpConnection(m_eastPort, m_terminalPort);
		if (m_eastPort.m_outputPortSwitched == PortType::NorthPort)
			m_crossbar.setUpConnection(m_eastPort, m_northPort);
		if (m_eastPort.m_outputPortSwitched == PortType::SouthPort)
			m_crossbar.setUpConnection(m_eastPort, m_southPort);
		if (m_eastPort.m_outputPortSwitched == PortType::WestPort)
			m_crossbar.setUpConnection(m_eastPort, m_westPort);
	}
}

void Router::transmitCredits()
{
	for (auto& connection : m_crossbar.m_connections)
	{
		if (connection.first->m_portType == PortType::NorthPort)
		{
			Credit credit{ m_northPort.m_virtualChannelSwitched };
			m_northPort.m_outCreditRegister.push_back(credit);
			logDebug(" Crossbar: credit sent in north port ");
		}

		if (connection.first->m_portType == PortType::SouthPort)
		{
			Credit credit{ m_southPort.m_virtualChannelSwitched };
			m_southPort.m_outCreditRegister.push_back(credit);
			logDebug(" Crossbar: credit sent in south port ");
		}

		if (connection.first->m_portType == PortType::WestPort)
		{
			Credit credit{ m_westPort.m_virtualChannelSwitched };
			m_westPort.m_outCreditRegister.push_back(credit);
			logDebug(" Crossbar: credit sent in west port ");
		}

		if (connection.first->m_portType == PortType::EastPort)
		{
			Credit credit{ m_eastPort.m_virtualChannelSwitched };
			m_eastPort.m_outCreditRegister.push_back(credit);
			logDebug(" Crossbar: credit sent in east port ");
		}
	}
}

void Router::resetFields()
{
	for (auto& connection : m_crossbar.m_connections)
	{
		if (connection.first->m_portType == PortType::TerminalPort)
		{
			if (connection.second->m_outFlitRegister.front().flitType == FlitType::HeadTailFlit
				|| connection.second->m_outFlitRegister.front().flitType == FlitType::TailFlit)
			{
				//m_terminalPort.m_localClock.tickTriggerClock(PERIOD_ROUTER_ST);

				resetRCVAOutputFields(m_terminalPort.m_virtualChannel.m_outputPortRouted, m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated);
				resetRCVAInputFields(PortType::TerminalPort, m_terminalPort.m_virtualChannelSwitched);
			}
			resetSAFields(PortType::TerminalPort);
		}

		if (connection.first->m_portType == PortType::NorthPort)
		{
			if (connection.second->m_outFlitRegister.front().flitType == FlitType::HeadTailFlit
				|| connection.second->m_outFlitRegister.front().flitType == FlitType::TailFlit)
			{
				//m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_localClock.tickTriggerClock(PERIOD_ROUTER_ST);

				resetRCVAOutputFields(m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_outputPortRouted, m_northPort.m_virtualChannels.at(m_northPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated);
				resetRCVAInputFields(PortType::NorthPort, m_northPort.m_virtualChannelSwitched);
			}
			resetSAFields(PortType::NorthPort);
		}

		if (connection.first->m_portType == PortType::SouthPort)
		{
			if (connection.second->m_outFlitRegister.front().flitType == FlitType::HeadTailFlit
				|| connection.second->m_outFlitRegister.front().flitType == FlitType::TailFlit)
			{
				//m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_localClock.tickTriggerClock(PERIOD_ROUTER_ST);

				resetRCVAOutputFields(m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_outputPortRouted, m_southPort.m_virtualChannels.at(m_southPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated);
				resetRCVAInputFields(PortType::SouthPort, m_southPort.m_virtualChannelSwitched);
			}
			resetSAFields(PortType::SouthPort);
		}

		if (connection.first->m_portType == PortType::WestPort)
		{
			if (connection.second->m_outFlitRegister.front().flitType == FlitType::HeadTailFlit
				|| connection.second->m_outFlitRegister.front().flitType == FlitType::TailFlit)
			{
				//m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_localClock.tickTriggerClock(PERIOD_ROUTER_ST);

				resetRCVAOutputFields(m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_outputPortRouted, m_westPort.m_virtualChannels.at(m_westPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated);
				resetRCVAInputFields(PortType::WestPort, m_westPort.m_virtualChannelSwitched);
			}
			resetSAFields(PortType::WestPort);
		}

		if (connection.first->m_portType == PortType::EastPort)
		{
			if (connection.second->m_outFlitRegister.front().flitType == FlitType::HeadTailFlit
				|| connection.second->m_outFlitRegister.front().flitType == FlitType::TailFlit)
			{
				//m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_localClock.tickTriggerClock(PERIOD_ROUTER_ST);

				resetRCVAOutputFields(m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_outputPortRouted, m_eastPort.m_virtualChannels.at(m_eastPort.m_virtualChannelSwitched).m_outputVirtualChannelAllocated);
				resetRCVAInputFields(PortType::EastPort, m_eastPort.m_virtualChannelSwitched);
			}
			resetSAFields(PortType::EastPort);
		}
	}
}

void Router::resetSAFields(const PortType port)
{
	if (port == PortType::TerminalPort)
	{
		m_terminalPort.m_virtualChannelSwitched = 0;
		m_terminalPort.m_outputPortSwitched = PortType::Unselected;
	}

	if (port == PortType::NorthPort)
	{
		m_northPort.m_virtualChannelSwitched = 0;
		m_northPort.m_outputPortSwitched = PortType::Unselected;
	}

	if (port == PortType::SouthPort)
	{
		m_southPort.m_virtualChannelSwitched = 0;
		m_southPort.m_outputPortSwitched = PortType::Unselected;
	}

	if (port == PortType::WestPort)
	{
		m_westPort.m_virtualChannelSwitched = 0;
		m_westPort.m_outputPortSwitched = PortType::Unselected;
	}

	if (port == PortType::EastPort)
	{
		m_eastPort.m_virtualChannelSwitched = 0;
		m_eastPort.m_outputPortSwitched = PortType::Unselected;
	}
}

void Router::resetRCVAInputFields(const PortType port, const int virtualChannel)
{
	if (port == PortType::TerminalPort)
	{
		m_terminalPort.m_virtualChannel.m_virtualChannelState = VirtualChannelState::I;
		m_terminalPort.m_virtualChannel.m_outputPortRouted = PortType::Unselected;
		m_terminalPort.m_virtualChannel.m_outputVirtualChannelAllocated = -1;
	}

	if (port == PortType::NorthPort)
	{
		m_northPort.m_virtualChannels.at(virtualChannel).m_virtualChannelState = VirtualChannelState::I;
		m_northPort.m_virtualChannels.at(virtualChannel).m_outputPortRouted = PortType::Unselected;
		m_northPort.m_virtualChannels.at(virtualChannel).m_outputVirtualChannelAllocated = -1;
	}

	if (port == PortType::SouthPort)
	{
		m_southPort.m_virtualChannels.at(virtualChannel).m_virtualChannelState = VirtualChannelState::I;
		m_southPort.m_virtualChannels.at(virtualChannel).m_outputPortRouted = PortType::Unselected;
		m_southPort.m_virtualChannels.at(virtualChannel).m_outputVirtualChannelAllocated = -1;
	}

	if (port == PortType::WestPort)
	{
		m_westPort.m_virtualChannels.at(virtualChannel).m_virtualChannelState = VirtualChannelState::I;
		m_westPort.m_virtualChannels.at(virtualChannel).m_outputPortRouted = PortType::Unselected;
		m_westPort.m_virtualChannels.at(virtualChannel).m_outputVirtualChannelAllocated = -1;
	}

	if (port == PortType::EastPort)
	{
		m_eastPort.m_virtualChannels.at(virtualChannel).m_virtualChannelState = VirtualChannelState::I;
		m_eastPort.m_virtualChannels.at(virtualChannel).m_outputPortRouted = PortType::Unselected;
		m_eastPort.m_virtualChannels.at(virtualChannel).m_outputVirtualChannelAllocated = -1;
	}
}

void Router::resetRCVAOutputFields(const PortType port, const int virtualChannel)
{
	if (port == PortType::TerminalPort)
	{
		m_terminalPort.m_downstreamVirtualChannelState = VirtualChannelState::I;
		m_terminalPort.m_inputPortRouted = PortType::Unselected;
		m_terminalPort.m_inputVirtualChannelAllocated = -1;
	}

	if (port == PortType::NorthPort)
	{
		m_northPort.m_inputPortRouted.at(virtualChannel) = PortType::Unselected;
		m_northPort.m_inputVirtualChannelAllocated.at(virtualChannel) = -1;
	}

	if (port == PortType::SouthPort)
	{
		m_southPort.m_inputPortRouted.at(virtualChannel) = PortType::Unselected;
		m_southPort.m_inputVirtualChannelAllocated.at(virtualChannel) = -1;
	}

	if (port == PortType::WestPort)
	{
		m_westPort.m_inputPortRouted.at(virtualChannel) = PortType::Unselected;
		m_westPort.m_inputVirtualChannelAllocated.at(virtualChannel) = -1;
	}

	if (port == PortType::EastPort)
	{
		m_eastPort.m_inputPortRouted.at(virtualChannel) = PortType::Unselected;
		m_eastPort.m_inputVirtualChannelAllocated.at(virtualChannel) = -1;
	}
}

void Router::traverseThisSwitchThenAllocateNextSwitch()
{
	traverseSwitch();

	allocateSwitch();
}

void Router::synchronizeTriggerClocks()
{
	m_terminalPort.m_localClock.synchronizeTriggerClock();
	for (int i{}; i < VC_NUMBER; ++i)
	{
		m_northPort.m_virtualChannels.at(i).m_localClock.synchronizeTriggerClock();
		m_southPort.m_virtualChannels.at(i).m_localClock.synchronizeTriggerClock();
		m_westPort.m_virtualChannels.at(i).m_localClock.synchronizeTriggerClock();
		m_eastPort.m_virtualChannels.at(i).m_localClock.synchronizeTriggerClock();
	}
}

void Router::synchronizeExecutionClocks()
{
	m_terminalPort.m_localClock.synchronizeExecutionClock();
	for (int i{}; i < VC_NUMBER; ++i)
	{
		m_northPort.m_virtualChannels.at(i).m_localClock.synchronizeExecutionClock();
		m_southPort.m_virtualChannels.at(i).m_localClock.synchronizeExecutionClock();
		m_westPort.m_virtualChannels.at(i).m_localClock.synchronizeExecutionClock();
		m_eastPort.m_virtualChannels.at(i).m_localClock.synchronizeExecutionClock();
	}
}

void Router::updateEnable()
{
	m_terminalPort.updateEnable();
	m_northPort.updateEnable();
	m_southPort.updateEnable();
	m_westPort.updateEnable();
	m_eastPort.updateEnable();
}

void Router::viewData()
{
#if defined (DEBUG)
	// view terminal port
	viewTerminalPortData();

	// view north port
	viewNorthPortData();

	// view south port
	viewSouthPortData();

	// view west port
	viewWestPortData();

	// view east port
	viewEastPortData();
#endif
}

void Router::viewTerminalPortData()
{
	log(" -------------------------------------- ");
	log(" Terminal port data: ");

	log(" In flit register: ");
	for (int i{}; i < m_terminalPort.m_inFlitRegister.size(); ++i)
		std::cout << " | " << m_terminalPort.m_inFlitRegister.at(i).xID << m_terminalPort.m_inFlitRegister.at(i).flitType << m_terminalPort.m_inFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Virtual channel: ");
	std::cout << " | "  << m_terminalPort.m_virtualChannel.m_virtualChannelState;
	for (int i{}; i < m_terminalPort.m_virtualChannel.m_buffer.size(); ++i)
		std::cout << " | " << m_terminalPort.m_virtualChannel.m_buffer.at(i).xID << m_terminalPort.m_virtualChannel.m_buffer.at(i).flitType << m_terminalPort.m_virtualChannel.m_buffer.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Crossbar input register: ");
	for (int i{}; i < m_terminalPort.m_crossbarInputRegister.size(); ++i)
		std::cout << " | " << m_terminalPort.m_crossbarInputRegister.at(i).xID << m_terminalPort.m_crossbarInputRegister.at(i).flitType << m_terminalPort.m_crossbarInputRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Out flit register: ");
	for (int i{}; i < m_terminalPort.m_outFlitRegister.size(); ++i)
		std::cout << " | " << m_terminalPort.m_outFlitRegister.at(i).xID << m_terminalPort.m_outFlitRegister.at(i).flitType << m_terminalPort.m_outFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" -------------------------------------- ");
}

void Router::viewNorthPortData()
{
	log(" North port data: ");

	log(" In flit register: ");
	for (int i{}; i < m_northPort.m_inFlitRegister.size(); ++i)
		std::cout << " | " << m_northPort.m_inFlitRegister.at(i).xID << m_northPort.m_inFlitRegister.at(i).flitType << m_northPort.m_inFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Virtual channels: ");
	for (int i{}; i < VC_NUMBER; ++i)
	{
		std::cout << " | " << m_northPort.m_virtualChannels.at(i).m_virtualChannelState;
		for (int j{}; j < m_northPort.m_virtualChannels.at(i).m_buffer.size(); ++j)
			std::cout << " | " << m_northPort.m_virtualChannels.at(i).m_buffer.at(j).xID << m_northPort.m_virtualChannels.at(i).m_buffer.at(j).flitType << m_northPort.m_virtualChannels.at(i).m_buffer.at(j).bodyFlitNo;
		std::cout << " | " << std::endl;
	}

	log(" Crossbar input register: ");
	for (int i{}; i < m_northPort.m_crossbarInputRegister.size(); ++i)
		std::cout << " | " << m_northPort.m_crossbarInputRegister.at(i).xID << m_northPort.m_crossbarInputRegister.at(i).flitType << m_northPort.m_crossbarInputRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Out flit register: ");
	for (int i{}; i < m_northPort.m_outFlitRegister.size(); ++i)
		std::cout << " | " << m_northPort.m_outFlitRegister.at(i).xID << m_northPort.m_outFlitRegister.at(i).flitType << m_northPort.m_outFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;
	log(" -------------------------------------- ");
}

void Router::viewSouthPortData()
{
	log(" South port data: ");

	log(" In flit register: ");
	for (int i{}; i < m_southPort.m_inFlitRegister.size(); ++i)
		std::cout << " | " << m_southPort.m_inFlitRegister.at(i).xID << m_southPort.m_inFlitRegister.at(i).flitType << m_southPort.m_inFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Virtual channels: ");
	for (int i{}; i < VC_NUMBER; ++i)
	{
		std::cout << " | " << m_southPort.m_virtualChannels.at(i).m_virtualChannelState;
		for (int j{}; j < m_southPort.m_virtualChannels.at(i).m_buffer.size(); ++j)
			std::cout << " | " << m_southPort.m_virtualChannels.at(i).m_buffer.at(j).xID << m_southPort.m_virtualChannels.at(i).m_buffer.at(j).flitType << m_southPort.m_virtualChannels.at(i).m_buffer.at(j).bodyFlitNo;
		std::cout << " | " << std::endl;
	}

	log(" Crossbar input register: ");
	for (int i{}; i < m_southPort.m_crossbarInputRegister.size(); ++i)
		std::cout << " | " << m_southPort.m_crossbarInputRegister.at(i).xID << m_southPort.m_crossbarInputRegister.at(i).flitType << m_southPort.m_crossbarInputRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Out flit register: ");
	for (int i{}; i < m_southPort.m_outFlitRegister.size(); ++i)
		std::cout << " | " << m_southPort.m_outFlitRegister.at(i).xID << m_southPort.m_outFlitRegister.at(i).flitType << m_southPort.m_outFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;
	log(" -------------------------------------- ");
}

void Router::viewWestPortData()
{
	log(" West port data: ");

	log(" In flit register: ");
	for (int i{}; i < m_westPort.m_inFlitRegister.size(); ++i)
		std::cout << " | " << m_westPort.m_inFlitRegister.at(i).xID << m_westPort.m_inFlitRegister.at(i).flitType << m_westPort.m_inFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Virtual channels: ");
	for (int i{}; i < VC_NUMBER; ++i)
	{
		std::cout << " | " << m_westPort.m_virtualChannels.at(i).m_virtualChannelState;
		for (int j{}; j < m_westPort.m_virtualChannels.at(i).m_buffer.size(); ++j)
			std::cout << " | " << m_westPort.m_virtualChannels.at(i).m_buffer.at(j).xID << m_westPort.m_virtualChannels.at(i).m_buffer.at(j).flitType << m_westPort.m_virtualChannels.at(i).m_buffer.at(j).bodyFlitNo;
		std::cout << " | " << std::endl;
	}

	log(" Crossbar input register: ");
	for (int i{}; i < m_westPort.m_crossbarInputRegister.size(); ++i)
		std::cout << " | " << m_westPort.m_crossbarInputRegister.at(i).xID << m_westPort.m_crossbarInputRegister.at(i).flitType << m_westPort.m_crossbarInputRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Out flit register: ");
	for (int i{}; i < m_westPort.m_outFlitRegister.size(); ++i)
		std::cout << " | " << m_westPort.m_outFlitRegister.at(i).xID << m_westPort.m_outFlitRegister.at(i).flitType << m_westPort.m_outFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;
	log(" -------------------------------------- ");
}

void Router::viewEastPortData()
{
	log(" East port data: ");

	log(" In flit register: ");
	for (int i{}; i < m_eastPort.m_inFlitRegister.size(); ++i)
		std::cout << " | " << m_eastPort.m_inFlitRegister.at(i).xID << m_eastPort.m_inFlitRegister.at(i).flitType << m_eastPort.m_inFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Virtual channels: ");
	for (int i{}; i < VC_NUMBER; ++i)
	{
		std::cout << " | " << m_eastPort.m_virtualChannels.at(i).m_virtualChannelState;
		for (int j{}; j < m_eastPort.m_virtualChannels.at(i).m_buffer.size(); ++j)
			std::cout << " | " << m_eastPort.m_virtualChannels.at(i).m_buffer.at(j).xID << m_eastPort.m_virtualChannels.at(i).m_buffer.at(j).flitType << m_eastPort.m_virtualChannels.at(i).m_buffer.at(j).bodyFlitNo;
		std::cout << " | " << std::endl;
	}

	log(" Crossbar input register: ");
	for (int i{}; i < m_eastPort.m_crossbarInputRegister.size(); ++i)
		std::cout << " | " << m_eastPort.m_crossbarInputRegister.at(i).xID << m_eastPort.m_crossbarInputRegister.at(i).flitType << m_eastPort.m_crossbarInputRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;

	log(" Out flit register: ");
	for (int i{}; i < m_eastPort.m_outFlitRegister.size(); ++i)
		std::cout << " | " << m_eastPort.m_outFlitRegister.at(i).xID << m_eastPort.m_outFlitRegister.at(i).flitType << m_eastPort.m_outFlitRegister.at(i).bodyFlitNo;
	std::cout << " | " << std::endl;
	log(" -------------------------------------- ");
}