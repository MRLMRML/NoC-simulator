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
		if (!connection.first->outBuffer.empty())
		{
			connection.second->inBuffer.push_back(connection.first->outBuffer.front());
			log(" Links: packet transferred (LHS port outBuffer -> RHS port inBuffer)");
		}
		if (!connection.second->outBuffer.empty())
		{
			connection.first->inBuffer.push_back(connection.second->outBuffer.front());
			log(" Links: packet transferred (LHS port inBuffer <- RHS port outBuffer)");
		}
	}

	for (auto& connection : m_connections)
	{
		if (!connection.first->outBuffer.empty())
			connection.first->outBuffer.pop_front();
		if (!connection.second->outBuffer.empty())
			connection.second->outBuffer.pop_front();
	}
}