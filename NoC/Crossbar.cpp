#include "Crossbar.h"

void Crossbar::setUpConnection(Port& inputPort, Port& outputPort)
{
	m_connections.insert({ &inputPort, &outputPort });
	log(" Crossbar: connection set up ");
}

void Crossbar::terminateConnection(Port& inputPort, Port& outputPort)
{
	m_connections.erase({ &inputPort, &outputPort });
	log(" Crossbar: connection terminated ");
}

void Crossbar::terminateConnection(const std::pair<Port*, Port*>& connection)
{
	m_connections.erase(connection);
	log(" Crossbar: connection terminated ");
}

void Crossbar::terminateAllConnections()
{
	if (!m_connections.empty())
	{
		m_connections.clear();
		log(" Crossbar: all connections terminated ");
	}
}

void Crossbar::traversal()
{
	// transfer one element in Port pair; one way
	for (auto& connection : m_connections)
	{
		connection.second->m_outFlitRegister.push_back(connection.first->m_crossbarInputRegister.front());
		connection.first->m_crossbarInputRegister.pop_front();
		log(" Crossbar: flit transferred (crossbar input register -> out flit register)");
	}
}