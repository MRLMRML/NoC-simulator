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
		if (!connection.first->outFlitBuffer.empty())
		{
			connection.second->inFlitBuffer.push_back(connection.first->outFlitBuffer.front());
			log(" Links: flit transferred (LHS -> RHS)");
		}

		if (!connection.second->outCreditBuffer.empty())
		{
			connection.first->inCreditBuffer.push_back(connection.second->outCreditBuffer.front());
			log(" Links: credit transferred (LHS <- RHS)");
		}

		if (!connection.second->outFlitBuffer.empty())
		{
			connection.first->inFlitBuffer.push_back(connection.second->outFlitBuffer.front());
			log(" Links: flit transferred (LHS <- RHS)");
		}

		if (!connection.first->outCreditBuffer.empty())
		{
			connection.second->inCreditBuffer.push_back(connection.first->outCreditBuffer.front());
			log(" Links: credit transferred (LHS <- RHS)");
		}
	}

	for (auto& connection : m_connections)
	{
		if (!connection.first->outFlitBuffer.empty())
			connection.first->outFlitBuffer.pop_front();
		if (!connection.second->outCreditBuffer.empty())
			connection.second->outCreditBuffer.pop_front();
		if (!connection.second->outFlitBuffer.empty())
			connection.second->outFlitBuffer.pop_front();
		if (!connection.first->outCreditBuffer.empty())
			connection.first->outCreditBuffer.pop_front();
	}
}