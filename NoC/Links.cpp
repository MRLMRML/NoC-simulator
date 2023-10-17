#include "Links.h"

void Links::setUpConnection(Port& inputPort, Port& outputPort)
{
	Link link{ inputPort, outputPort };
	m_connections.insert(link);
	log(" Links: connection set up ");
}

void Links::terminateConnection(Port& inputPort, Port& outputPort)
{
	Link link{ inputPort, outputPort };
	m_connections.erase(link);
	log(" Links: connection terminated ");
}

void Links::terminateAllConnections()
{
	m_connections.clear();
	log(" Links: all connections terminated ");
}

void Links::updateEnable()
{
	for (auto& connection : m_connections)
	{
		connection.updateEnable();
	}
}

//bool Links::areEmpty()
//{
//	bool areEmpty{ true };
//	for (auto& connection : m_connections)
//	{
//		areEmpty = areEmpty && connection.first->isEmpty() && connection.second->isEmpty();
//	}
//	return areEmpty;
//}

void Links::runOneStep()
{
	// transfer one element in Port pair bidirectionaliy
	for (auto& connection : m_connections)
	{
		if (connection.m_localClock.triggerLocalEvent())
		{
			if (connection.m_enable)
			{
				if (!connection.m_connection.first->m_outFlitRegister.empty()
					&& connection.m_connection.second->m_inFlitRegister.size() < REGISTER_DEPTH)
				{
					connection.m_connection.second->m_inFlitRegister.push_back(connection.m_connection.first->m_outFlitRegister.front());
					connection.m_connection.first->m_outFlitRegister.pop_front();
					log(" Links: flit transferred (LHS -> RHS) ");
				}

				if (!connection.m_connection.second->m_outCreditRegister.empty()
					&& connection.m_connection.first->m_inCreditRegister.size() < REGISTER_DEPTH)
				{
					connection.m_connection.first->m_inCreditRegister.push_back(connection.m_connection.second->m_outCreditRegister.front());
					connection.m_connection.second->m_outCreditRegister.pop_front();
					log(" Links: credit transferred (LHS <- RHS) ");
				}

				if (!connection.m_connection.second->m_outFlitRegister.empty()
					&& connection.m_connection.first->m_inFlitRegister.size() < REGISTER_DEPTH)
				{
					connection.m_connection.first->m_inFlitRegister.push_back(connection.m_connection.second->m_outFlitRegister.front());
					connection.m_connection.second->m_outFlitRegister.pop_front();
					log(" Links: flit transferred (LHS <- RHS) ");
				}

				if (!connection.m_connection.first->m_outCreditRegister.empty()
					&& connection.m_connection.second->m_inCreditRegister.size() < REGISTER_DEPTH)
				{
					connection.m_connection.second->m_inCreditRegister.push_back(connection.m_connection.first->m_outCreditRegister.front());
					connection.m_connection.first->m_outCreditRegister.pop_front();
					log(" Links: credit transferred (LHS -> RHS) ");
				}

				connection.m_localClock.tickLocalClock(CYCLES_LINK);
			}
			else
				connection.m_localClock.synchronizeClock();
		}
	}
}