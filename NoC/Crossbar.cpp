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
		if (connection.second->m_outFlitRegister.empty())
		{
			connection.second->m_outFlitRegister.push_back(connection.first->m_crossbarInputRegister.front());
			connection.first->m_crossbarInputRegister.pop_front();
			log(" Crossbar: flit transferred (crossbar input register -> out flit register)");
		}

		else
			throw std::runtime_error{ " Crossbar: out flit register is not set to false " };
	}

	//// create a set that contains input ports only
	//std::set<Port*> inputPorts{};
	//for (auto& connection : m_connections)
	//{
	//	inputPorts.insert(connection.first);
	//}

	//// pop one element for every unique input port; since one input can connect to multiple outputs
	//for (auto& inputPort : inputPorts) // 
	//{
	//	if (!inputPort->outBuffer.empty())
	//		inputPort->outBuffer.pop_front();
	//}

	//// pop one element for every output port; one outputPort only has one inputPort connection
	//for (auto& connection : m_connections)
	//{
	//	if (!connection.second->outBuffer.empty())
	//		connection.second->outBuffer.pop_front();
	//}
}