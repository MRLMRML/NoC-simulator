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

	void updateEnable();

	//bool areEmpty();
	
	// Timing
	void synchronizeTriggerClocks();
	void synchronizeExecutionClocks();

	void runOneStep();
private:
	std::set<Link> m_connections{};
};