#include "Links.h"

void Links::setUpConnection(Port& inputPort, Port& outputPort)
{
	Link link{ inputPort, outputPort };
	m_connections.insert(link);
	logDebug(" Links: connection set up ");
}

void Links::terminateConnection(Port& inputPort, Port& outputPort)
{
	Link link{ inputPort, outputPort };
	m_connections.erase(link);
	logDebug(" Links: connection terminated ");
}

void Links::terminateAllConnections()
{
	m_connections.clear();
	logDebug(" Links: all connections terminated ");
}

void Links::updateEnable()
{
	for (auto& connection : m_connections)
	{
		connection.updateEnable();
	}
}

void Links::synchronizeTriggerClocks()
{
	for (auto& connection : m_connections)
	{
		connection.m_localClock.synchronizeTriggerClock();
	}
}

void Links::synchronizeExecutionClocks()
{
	for (auto& connection : m_connections)
	{
		connection.m_localClock.synchronizeExecutionClock();
	}
}

void Links::runOneStep()
{
	// transfer one element in Port pair bidirectionaliy
	for (auto& connection : m_connections)
	{
		if (connection.m_localClock.triggerLocalEvent())
		{
			if (connection.m_firstOutFlitEnable
				|| connection.m_firstOutCreditEnable
				|| connection.m_secondOutFlitEnable
				|| connection.m_secondOutCreditEnable)
			{
				if (!connection.m_localClock.isWaitingForExecution())
				{
					connection.m_localClock.tickExecutionClock(EXECUTION_TIME_LINK - 1);
					connection.m_localClock.toggleWaitingForExecution();
				}

				if (connection.m_localClock.executeLocalEvent())
				{
					if (connection.m_firstOutFlitEnable
						&& connection.m_connection.second->m_inFlitRegister.size() < REGISTER_DEPTH)
					{
						connection.m_connection.second->m_inFlitRegister.push_back(connection.m_connection.first->m_outFlitRegister.front());
						connection.m_connection.first->m_outFlitRegister.pop_front();
						logDebug(" Links: flit transferred (LHS -> RHS) ");
					}

					if (connection.m_firstOutCreditEnable
						&& connection.m_connection.second->m_inCreditRegister.size() < REGISTER_DEPTH)
					{
						connection.m_connection.second->m_inCreditRegister.push_back(connection.m_connection.first->m_outCreditRegister.front());
						connection.m_connection.first->m_outCreditRegister.pop_front();
						logDebug(" Links: credit transferred (LHS -> RHS) ");
					}

					if (connection.m_secondOutFlitEnable
						&& connection.m_connection.first->m_inFlitRegister.size() < REGISTER_DEPTH)
					{
						connection.m_connection.first->m_inFlitRegister.push_back(connection.m_connection.second->m_outFlitRegister.front());
						connection.m_connection.second->m_outFlitRegister.pop_front();
						logDebug(" Links: flit transferred (LHS <- RHS) ");
					}

					if (connection.m_secondOutCreditEnable
						&& connection.m_connection.first->m_inCreditRegister.size() < REGISTER_DEPTH)
					{
						connection.m_connection.first->m_inCreditRegister.push_back(connection.m_connection.second->m_outCreditRegister.front());
						connection.m_connection.second->m_outCreditRegister.pop_front();
						logDebug(" Links: credit transferred (LHS <- RHS) ");
					}

					connection.m_localClock.tickTriggerClock(PERIOD_LINK - EXECUTION_TIME_LINK + 1);
					connection.m_localClock.tickExecutionClock(PERIOD_LINK - EXECUTION_TIME_LINK + 1);
					connection.m_localClock.toggleWaitingForExecution();
				}
			}
		}
	}
	synchronizeTriggerClocks();
	synchronizeExecutionClocks();
}