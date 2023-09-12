#include "Links.h"

void Links::setUpConnection(Port& inputPort, Port& outputPort)
{
	m_connections.insert({ &inputPort, &outputPort });
	log(" Links: connection set up ");
}

void Links::terminateConnection(Port& inputPort, Port& outputPort)
{
	m_connections.erase({ &inputPort, &outputPort });
	log(" Links: connection terminated ");
}

void Links::terminateAllConnections()
{
	m_connections.clear();
	log(" Links: all connections terminated ");
}

bool Links::areEmpty()
{
	bool areEmpty{ true };
	for (auto& connection : m_connections)
	{
		areEmpty = areEmpty && connection.first->isEmpty() && connection.second->isEmpty();
	}
	return areEmpty;
}

void Links::runOneStep()
{
	// transfer one element in Port pair
	for (auto& connection : m_connections)
	{
		if (!connection.first->m_outFlitBuffer.empty())
		{
			connection.second->m_inFlitBuffer.push_back(connection.first->m_outFlitBuffer.front());
			log(" Links: flit transferred (LHS -> RHS)");
		}

		if (!connection.second->m_outCreditBuffer.empty())
		{
			connection.first->m_inCreditBuffer.push_back(connection.second->m_outCreditBuffer.front());
			log(" Links: credit transferred (LHS <- RHS)");
		}

		if (!connection.second->m_outFlitBuffer.empty())
		{
			connection.first->m_inFlitBuffer.push_back(connection.second->m_outFlitBuffer.front());
			log(" Links: flit transferred (LHS <- RHS)");
		}

		if (!connection.first->m_outCreditBuffer.empty())
		{
			connection.second->m_inCreditBuffer.push_back(connection.first->m_outCreditBuffer.front());
			log(" Links: credit transferred (LHS <- RHS)");
		}
	}

	for (auto& connection : m_connections)
	{
		if (!connection.first->m_outFlitBuffer.empty())
			connection.first->m_outFlitBuffer.pop_front();
		if (!connection.second->m_outCreditBuffer.empty())
			connection.second->m_outCreditBuffer.pop_front();
		if (!connection.second->m_outFlitBuffer.empty())
			connection.second->m_outFlitBuffer.pop_front();
		if (!connection.first->m_outCreditBuffer.empty())
			connection.first->m_outCreditBuffer.pop_front();
	}
}