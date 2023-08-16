#pragma once
#include "Router.h"
#include "Links.h"
//#include "Node.h"
#include <array>

//extern size_t globalCycle;

class Network
{
public:
	Network()
	{
		for (int i{}; i < m_routers.size(); ++i)
			m_routers[i].m_routerID = { i / NETWORK_DIMENSION_X, i % NETWORK_DIMENSION_X };
	};

	void setUpConnections();

	void printRouterIDs();

	//void mountNode(const RouterID& routerID, const Node* node);

	void printNodeIDs();

	void viewMappingTable();

	void updateMappingTables();

	void terminateConnections();

	bool isQuiet();

	void runOneStep();

private:
	std::array<Router, ROUTER_NUMBER> m_routers{};
	Links m_links{};
	std::vector<MappingTableLine> m_mappingTable{};
	size_t m_networkCycle{};
};