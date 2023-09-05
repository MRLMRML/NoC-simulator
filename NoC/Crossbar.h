#pragma once
#include "Port.h"
#include "Log.h"
#include <utility>
#include <set>

class Crossbar
{
public:
	Crossbar() = default;

	void setUpConnection(Port& inputPort, Port& outputPort);

	void terminateConnection(Port& inputPort, Port& outputPort);

	void terminateAllConnections();

	void runOneStep();

private:
	std::set<std::pair<Port*, Port*>> m_connections{};
};