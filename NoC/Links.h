#pragma once
#include "Link.h"
#include "Log.h"
#include <set>

class Links
{
public:
	Links() = default;

	void setUpConnection(Port& inputPort, Port& outputPort);

	void terminateConnection(Port& inputPort, Port& outputPort);

	void terminateAllConnections();

	//bool areEmpty();

	void runOneStep();

	// Timing
	//Clock m_localClock{};
private:
	std::set<Link> m_connections{};
};